////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////
/////
///// blockfrost integration
/////
/////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////
////////// 
/////



#include "blockfrost_subsystem.h"
#include "blockfrost_settings.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////
////////// 
/////



/////
///// SUBSYSTEM
/////////////////////////////
////////// 
/////



// INITIALIZE 
////////////////////////////////////////
void Ublockfrost_subsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    // Parent Call
    Super::Initialize(Collection);

    // Log
    UE_LOG(LogTemp, Warning, TEXT("blockfrost Subsystem Initialized"));

}



// GET VALID JSON OBJECT
////////////////////////////////////////
bool Ublockfrost_subsystem::GetValidJsonObject(FHttpResponsePtr Response, TSharedRef<TJsonReader<>>& OutReader, TSharedPtr<FJsonObject> OutJsonObject)
{
    // Check if response is valid
    if (!Response.IsValid())
    {
        // Log error
        UE_LOG(LogTemp, Error, TEXT("Response is not valid"));
        return false;
    }

    // Valid Response
    if (EHttpResponseCodes::IsOk(Response->GetResponseCode()))
    {
        // Response Content
        FString ResponseJson = Response->GetContentAsString();

        // Read json data
        TSharedPtr<FJsonObject> JsonObject;
        OutReader = TJsonReaderFactory<>::Create(ResponseJson);

        // Return true if deserialization is successful
        if (FJsonSerializer::Deserialize(OutReader, JsonObject) && JsonObject.IsValid())
        {
            OutJsonObject = JsonObject;
            return true;
        }
        // Log error
        UE_LOG(LogTemp, Error, TEXT("Invalid Response - Couldn't Deserialize Json Object"));
        return false;
    }
    // Is Not Okay
    UE_LOG(LogTemp, Error, TEXT("Invalid Response - Is Not Ok"));
    return false;
}



// GET VALID JSON ARRAY
////////////////////////////////////////
bool Ublockfrost_subsystem::GetValidJsonArray(FHttpResponsePtr Response, TSharedRef<TJsonReader<>>& OutReader, TArray<TSharedPtr<FJsonValue>>& OutJsonArray)
{
    // Check if response is valid
    if (!Response.IsValid())
    {
        // Log error
        UE_LOG(LogTemp, Error, TEXT("Response is not valid"));
        return false;
    }

    // Valid Response
    if (EHttpResponseCodes::IsOk(Response->GetResponseCode()))
    {
        // Response Content
        FString ResponseJson = Response->GetContentAsString();

        // Read json data
        TArray<TSharedPtr<FJsonValue>> JsonArray;
        OutReader = TJsonReaderFactory<>::Create(ResponseJson);

        // Return Json Array if deserialization is successful
        if (FJsonSerializer::Deserialize(OutReader, JsonArray) && JsonArray.Num() > 0)
        {
            OutJsonArray = JsonArray;
            return true;
        }
        // Log error
        UE_LOG(LogTemp, Error, TEXT("Invalid Response - Couldn't Deserialize Json Object"));

        return false;
    }
    // Is Not Okay
    UE_LOG(LogTemp, Error, TEXT("Invalid Response - Is Not Ok"));

    return false;
}



// MAKE BLOCKFROST REQUEST
////////////////////////////////////////
TSharedRef<IHttpRequest> Ublockfrost_subsystem::MakeBlockfrostRequest(const FName& ProjectName, const FString& RequestVerb, const FString& Endpoint)
{
    // Settings object
    Ublockfrost_settings* Settings = GetMutableDefault<Ublockfrost_settings>();

    // Get Project key using ProjectName & Settings::GetProjectKeyByName
    FString ProjectKey = Settings->GetProjectKeyByName(ProjectName);

    // Create HttpRequest Object
    TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();

    // If ProjectKey isn't empty, set Auth Header
    if (ProjectKey != "")
    {
        // Set Auth Header
        HttpRequest->SetHeader("project_id", *ProjectKey);

        // Get BaseURL from Settings
        FString RootEndpoint = Settings->RootEndpoint;

        // Create URL with BaseURL + Endpoint
        FString RequestURL = RootEndpoint + Endpoint;

        // Set URL
        HttpRequest->SetURL(*RequestURL);

        // Set as GET Request
        HttpRequest->SetVerb(*RequestVerb);

        // Return Request
        return HttpRequest;
    }

    // Log
    UE_LOG(LogTemp, Error, TEXT("blockfrost: Project key not found for project name: %s"), *ProjectName.ToString());

    return HttpRequest;
}



/////
/////  BLOCKFROST
/////////////////////////////
////////// 
/////



// ROOT ENDPOINT 
////////////////////////////////////////
// Request Root Endpoint
void Ublockfrost_subsystem::RequestRootEndpoint(const FName ProjectName)
{
    // Create HTTP Request
    TSharedRef<IHttpRequest> HttpRequest = MakeBlockfrostRequest(ProjectName, "GET", "/");

    // Bind On Request Complete
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &Ublockfrost_subsystem::ReceiveRootEndpoint);

    // Process Request
    HttpRequest->ProcessRequest();
}


// Receive Event for Backend Status
void Ublockfrost_subsystem::ReceiveRootEndpoint(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    // Json Reader
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(FString());

    // Json Object
    TSharedRef<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

    // If valid response
    if (GetValidJsonObject(Response, Reader, JsonObject))
    {
        // Build Response String
        FString ResponseString = JsonObject->GetStringField("url");

        // Call Delegate
        OnRootEndpoint.Broadcast(true, ResponseString);

        return;
    }
    // Call Delegate
    OnRootEndpoint.Broadcast(false, TEXT("Invalid Response"));
    
    return;
}





// BACKEND STATUS 
////////////////////////////////////////
// Request Backend Status
void Ublockfrost_subsystem::RequestBackendStatus(const FName ProjectName)
{
    // Create HTTP request
    TSharedRef<IHttpRequest> HttpRequest = MakeBlockfrostRequest(ProjectName, "GET", "/health");

    // Bind On Request Complete
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &Ublockfrost_subsystem::ReceiveBackendStatus);

    // Process Request
    HttpRequest->ProcessRequest();
}


// Receive Event for Backend Status
void Ublockfrost_subsystem::ReceiveBackendStatus(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    // Json Reader
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(FString());

    // Get Valid Response
    TSharedRef<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

    // If Valid Response
    if (GetValidJsonObject(Response, Reader, JsonObject))
    {
        // Call Delegate
        OnBackendStatus.Broadcast(true, JsonObject->GetBoolField(TEXT("is_healthy")));

        return;
    }
    // Call Delegate
    OnBackendStatus.Broadcast(false, false);

    return;
}



// BACKEND TIME
////////////////////////////////////////
// Request Backend Time
void Ublockfrost_subsystem::RequestBackendTime(const FName ProjectName)
{
    // Create HTTP request
    TSharedRef<IHttpRequest> HttpRequest = MakeBlockfrostRequest(ProjectName, "GET", "/health/clock");
    // Bind On Request Complete
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &Ublockfrost_subsystem::ReceiveBackendTime);

    // Process Request
    HttpRequest->ProcessRequest();
}


// Receive Event for Backend Time
void Ublockfrost_subsystem::ReceiveBackendTime(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    // Json Reader
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(FString());

    // Get Valid Response
    TSharedRef<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

    // If Valid Response
    if (GetValidJsonObject(Response, Reader, JsonObject))
    {
        // Get Server Time
        FString ServerTime = JsonObject->GetStringField("server_time");

        // Call Delegate
        OnBackendTime.Broadcast(true, FCString::Atoi64(*ServerTime));

        return;
    }
    // Call Delegate
    OnBackendTime.Broadcast(false, 0);

    return;
}



// USAGE METRICS
////////////////////////////////////////
// Request Usage Metrics
void Ublockfrost_subsystem::RequestUsageMetrics(const FName ProjectName)
{
    // Create HTTP Request
    TSharedRef<IHttpRequest> HttpRequest = MakeBlockfrostRequest(ProjectName, "GET", "/metrics");

    // Bind On Request Complete
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &Ublockfrost_subsystem::ReceiveUsageMetrics);

    // Process Request
    HttpRequest->ProcessRequest();
}


// Receive Event for Usage Metrics
void Ublockfrost_subsystem::ReceiveUsageMetrics(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    // Return Output - Usage Metrics Array
    TArray<FUsageMetricsStruct> UsageMetricsArray;

    // Json Array & Reader
    TArray<TSharedPtr<FJsonValue>> JsonArray;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(FString());

    // If Valid Response
    if (GetValidJsonArray(Response, Reader, JsonArray))
    {
        // Loop Entry Variables - JsonObject, Endpoint Metrics
        TSharedPtr<FJsonObject> JsonObject;
        FUsageMetricsStruct UsageMetrics;

        ///// LOOP JSONARRAY
        for (TSharedPtr<FJsonValue> JsonValue : JsonArray)
        {
            if (JsonValue.IsValid())
            {
                // Create JsonObject for current index
                JsonObject = JsonValue->AsObject();

                // Populate Struct
                UsageMetrics.Time = FCString::Atoi64(*JsonObject->GetStringField("time"));
                UsageMetrics.Calls = JsonObject->GetIntegerField("calls");

                // Add new entry to return array
                UsageMetricsArray.Add(UsageMetrics);
            }
        }
        // Call Delegate when finished looping
        OnUsageMetrics.Broadcast(true, UsageMetricsArray);

        return;
    }
    // Call Delegate when finished looping
    OnUsageMetrics.Broadcast(false, UsageMetricsArray);

    return;
}





// ENDPOINT USAGE METRICS
////////////////////////////////////////
// Request Endpoint Usage Metrics
void Ublockfrost_subsystem::RequestEndpointUsageMetrics(const FName ProjectName)
{
    // Create HTTP request
    TSharedRef<IHttpRequest> HttpRequest = MakeBlockfrostRequest(ProjectName, "GET", "/metrics/endpoints");

    // Bind On Request Complete
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &Ublockfrost_subsystem::ReceiveEndpointUsageMetrics);

    // Process Request
    HttpRequest->ProcessRequest();
}


// Receive Event for Endpoint Usage Metrics
void Ublockfrost_subsystem::ReceiveEndpointUsageMetrics(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    // Output - Endpoint Usage Metrics Array
    TArray<FEndpointUsageMetricsStruct> EndpointUsageMetricsArray;

    // Json Array & Reader
    TArray<TSharedPtr<FJsonValue>> JsonArray;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(FString());

    // If Valid Response
    if (GetValidJsonArray(Response, Reader, JsonArray))
    {
        // Loop Entry Variables - JsonObject, Endpoint Usage Metrics
        FEndpointUsageMetricsStruct EndpointUsageMetrics;
        TSharedPtr<FJsonObject> JsonObject;

        ///// LOOP JSONARRAY
        for (TSharedPtr<FJsonValue> JsonValue : JsonArray)
        {
            if (JsonValue.IsValid())
            {
                // Set JsonObject to Value for current index
                JsonObject = JsonValue->AsObject();

                // Populate Struct
                EndpointUsageMetrics.Time = FCString::Atoi64(*JsonObject->GetStringField("time"));
                EndpointUsageMetrics.Calls = JsonObject->GetIntegerField("calls");
                EndpointUsageMetrics.Endpoint = FName(JsonObject->GetStringField("endpoint"));

                // Add new entry to return array
                EndpointUsageMetricsArray.Add(EndpointUsageMetrics);
            }
        }
        // Call Delegate when finished looping
        OnEndpointUsageMetrics.Broadcast(true, EndpointUsageMetricsArray);

        return;
    }
    // Call Delegate
    OnEndpointUsageMetrics.Broadcast(false, EndpointUsageMetricsArray);
    
    return;
}




/////
/////  CARDANO
/////////////////////////////
////////// 
/////