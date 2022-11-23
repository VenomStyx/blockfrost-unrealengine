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



// MAKE BLOCKFROST REQUEST
////////////////////////////////////////
TSharedRef<IHttpRequest> Ublockfrost_subsystem::MakeBlockfrostRequest(const FName& ProjectName, const FString& RequestVerb, const FString& RequestURL)
{
    // Create HttpRequest Object
    TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();

    // Set URL
    HttpRequest->SetURL(*RequestURL);

    // Set as GET Request
    HttpRequest->SetVerb(*RequestVerb);

    // Settings object
    Ublockfrost_settings* Settings = GetMutableDefault<Ublockfrost_settings>();

    // Get Project key using ProjectName & Settings::GetProjectKeyByName
    FString ProjectKey = Settings->GetProjectKeyByName(ProjectName);

    // If ProjectKey isn't empty, set Auth
    if (ProjectKey != "")
    {
        // Set Project Key as Header
        HttpRequest->SetHeader("project_id", *ProjectKey);
    }
    // Else log error
    else
    {
        // Log
        UE_LOG(LogTemp, Error, TEXT("blockfrost: Project key not found for project name: %s"), *ProjectName.ToString());
    }

    // Return Request
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
    TSharedRef<IHttpRequest> HttpRequest = MakeBlockfrostRequest(ProjectName, TEXT("GET"), TEXT("https://cardano-mainnet.blockfrost.io/api/v0/"));

    // Bind On Request Complete
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &Ublockfrost_subsystem::ReceiveRootEndpoint);

    // Process Request
    HttpRequest->ProcessRequest();
}


// Receive Event for Root Endpoint
void Ublockfrost_subsystem::ReceiveRootEndpoint(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    ///// INVALID RESPONSE
    if (!Response.IsValid())
    {
        // Log
        UE_LOG(LogTemp, Error, TEXT("Response is not valid"));

        // Call Delegate
        OnRootEndpoint.Broadcast(false, TEXT("Invalid Response"));

        // Return
        return;
    }

    ///// VALID RESPONSE
    if (EHttpResponseCodes::IsOk(Response->GetResponseCode()))
    {
        // Response Content
        FString ResponseJson = Response->GetContentAsString();

        // Read json data
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseJson);

        // Assign bool to Deserialize Validity
        bool bIsValid = FJsonSerializer::Deserialize(Reader, JsonObject);

        // Log Validity State if invalid
        if (!bIsValid) { UE_LOG(LogTemp, Error, TEXT("Response is not valid")); }

        // Build Response String
        FString ResponseString = bIsValid ? JsonObject->GetStringField("url") : TEXT("Invalid Response");

        // Call Delegate
        OnRootEndpoint.Broadcast(bIsValid, ResponseString);

    }

    ///// INVALID RESPONSE
    else
    {
        // Log
        UE_LOG(LogTemp, Error, TEXT("Invalid Response - Is not Ok"));

        // Call Delegate
        OnRootEndpoint.Broadcast(false, TEXT("Invalid Response"));

        // Return
        return;
    }
}



// BACKEND STATUS 
////////////////////////////////////////
// Request Backend Status
void Ublockfrost_subsystem::RequestBackendStatus(const FName ProjectName)
{
    // Create HTTP request
    TSharedRef<IHttpRequest> HttpRequest = MakeBlockfrostRequest(ProjectName, TEXT("GET"), TEXT("https://cardano-mainnet.blockfrost.io/api/v0/health"));

    // Bind On Request Complete
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &Ublockfrost_subsystem::ReceiveBackendStatus);

    // Process Request
    HttpRequest->ProcessRequest();
}


// Receive Event for Backend Status
void Ublockfrost_subsystem::ReceiveBackendStatus(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    ///// INVALID RESPONSE
    if (!Response.IsValid())
    {
        // Log
        UE_LOG(LogTemp, Error, TEXT("Response is not valid"));

        // Call Delegate
        OnBackendStatus.Broadcast(false, false);

        // Return
        return;
    }

    ///// VALID RESPONSE
    if (EHttpResponseCodes::IsOk(Response->GetResponseCode()))
    {
        // Response Content
        FString ResponseJson = Response->GetContentAsString();

        // Read json data
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseJson);

        ///// VALID RESPONSE
        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            // Call Delegate
            OnBackendStatus.Broadcast(true, JsonObject->GetBoolField(TEXT("is_healthy")));

            // Return
            return;
        }

        ///// INVALID RESPONSE
        else
        {
            // Log
            UE_LOG(LogTemp, Error, TEXT("Invalid Response - Couldn't Deserialize Json Object"));

            // Call Delegate
            OnBackendStatus.Broadcast(false, false);

            // Return
            return;
        }
    }

    ///// INVALID RESPONSE
    else
    {
        // Log
        UE_LOG(LogTemp, Error, TEXT("Invalid Response - Is not Ok"));

        // Call Delegate
        OnBackendStatus.Broadcast(false, false);

        // Return
        return;
    }
}



// BACKEND TIME
////////////////////////////////////////
// Request Backend Time
void Ublockfrost_subsystem::RequestBackendTime(const FName ProjectName)
{
    // Create HTTP request
    TSharedRef<IHttpRequest> HttpRequest = MakeBlockfrostRequest(ProjectName, TEXT("GET"), TEXT("https://cardano-mainnet.blockfrost.io/api/v0/health/clock"));

    // Bind On Request Complete
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &Ublockfrost_subsystem::ReceiveBackendTime);

    // Process Request
    HttpRequest->ProcessRequest();
}


// Receive Event for Backend Time
void Ublockfrost_subsystem::ReceiveBackendTime(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    ///// INVALID RESPONSE
    if (!Response.IsValid())
    {
        // Log
        UE_LOG(LogTemp, Error, TEXT("Response is not valid"));

        // Call Delegate
        OnBackendTime.Broadcast(false, 0);

        // Return
        return;
    }

    ///// VALID RESPONSE
    if (EHttpResponseCodes::IsOk(Response->GetResponseCode()))
    {
        // Response Content
        FString ResponseJson = Response->GetContentAsString();

        // Read json data
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseJson);

        ///// VALID RESPONSE
        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            // Get Server Time
            FString ServerTime = JsonObject->GetStringField("server_time");

            // Call Delegate
            OnBackendTime.Broadcast(true, FCString::Atoi64(*ServerTime));

            // Return
            return;
        }

        ///// INVALID RESPONSE
        else
        {
            // Log
            UE_LOG(LogTemp, Error, TEXT("Invalid Response - Couldn't Deserialize Json Object"));

            // Call Delegate
            OnBackendTime.Broadcast(false, 0);

            // Return
            return;
        }
    }

    ///// INVALID RESPONSE
    else
    {
        // Log
        UE_LOG(LogTemp, Error, TEXT("Invalid Response - Is not Ok"));

        // Call Delegate
        OnBackendTime.Broadcast(false, 0);

        // Return
        return;
    }
}



// USAGE METRICS
////////////////////////////////////////
// Request Usage Metrics
void Ublockfrost_subsystem::RequestUsageMetrics(const FName ProjectName)
{
    // Create HTTP Request
    TSharedRef<IHttpRequest> HttpRequest = MakeBlockfrostRequest(ProjectName, TEXT("GET"), TEXT("https://cardano-mainnet.blockfrost.io/api/v0/metrics"));

    // Bind On Request Complete
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &Ublockfrost_subsystem::ReceiveUsageMetrics);

    // Process Request
    HttpRequest->ProcessRequest();
}


// Receive Event for Usage Metrics
void Ublockfrost_subsystem::ReceiveUsageMetrics(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    // Create return output struct array
    TArray<FUsageMetricsStruct> UsageMetricsArray;

    ///// INVALID RESPONSE
    if (!Response.IsValid())
    {
        // Log
        UE_LOG(LogTemp, Error, TEXT("Response is not valid"));

        // Call Delegate
        OnUsageMetrics.Broadcast(false, UsageMetricsArray);

        // Return
        return;
    }

    ///// VALID RESPONSE
    if (EHttpResponseCodes::IsOk(Response->GetResponseCode()))
    {
        // Get Response Content as String
        FString ResponseJson = Response->GetContentAsString();

        // Convert to JsonArray object & create JsonReader to read
        TArray<TSharedPtr<FJsonValue>> JsonArray;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseJson);

        ///// VALID JSON
        if (FJsonSerializer::Deserialize(Reader, JsonArray) && JsonArray.Num() > 0)
        {
            ///// LOOP JSONARRAY
            for (TSharedPtr<FJsonValue> JsonValue : JsonArray)
            {
                // Create JsonObject for current index
                TSharedPtr<FJsonObject> JsonObject = JsonValue->AsObject();

                // Create struct for output entry
                FUsageMetricsStruct UsageMetrics;

                // Get Response Info
                FString ServerTime = JsonObject->GetStringField("time");
                int32 Calls = JsonObject->GetIntegerField("calls");

                // Populate Struct
                UsageMetrics.Time = FCString::Atoi64(*ServerTime);
                UsageMetrics.Calls = JsonObject->GetIntegerField("calls");

                // Add new entry to return array
                UsageMetricsArray.Add(UsageMetrics);

            }
            // Call Delegate when finished looping
            OnUsageMetrics.Broadcast(true, UsageMetricsArray);

            // Complete
            return;
        }

        ///// INVALID RESPONSE
        else
        {
            // Log
            UE_LOG(LogTemp, Error, TEXT("Invalid Response - Couldn't Deserialize Json Object"));

            // Call Delegate
            OnUsageMetrics.Broadcast(false, UsageMetricsArray);

            // Complete
            return;
        }
    }

    ///// INVALID RESPONSE
    else
    {
        // Log
        UE_LOG(LogTemp, Error, TEXT("Invalid Response - Is not Ok"));

        // Call Delegate
        OnUsageMetrics.Broadcast(false, UsageMetricsArray);

        // Complete
        return;
    }
}



// ENDPOINT USAGE METRICS
////////////////////////////////////////
// Request Endpoint Usage Metrics
void Ublockfrost_subsystem::RequestEndpointUsageMetrics(const FName ProjectName)
{
    // Create HTTP request
    TSharedRef<IHttpRequest> HttpRequest = MakeBlockfrostRequest(ProjectName, TEXT("GET"), TEXT("https://cardano-mainnet.blockfrost.io/api/v0/metrics/endpoints"));

    // Bind On Request Complete
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &Ublockfrost_subsystem::ReceiveEndpointUsageMetrics);

    // Process Request
    HttpRequest->ProcessRequest();
}


// Receive Event for Endpoint Usage Metrics
void Ublockfrost_subsystem::ReceiveEndpointUsageMetrics(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{

    // Create return output struct array
    TArray<FEndpointUsageMetricsStruct> EndpointUsageMetricsArray;

    ///// INVALID RESPONSE
    if (!Response.IsValid())
    {
        // Log
        UE_LOG(LogTemp, Error, TEXT("Response is not valid"));

        // Call Delegate
        OnEndpointUsageMetrics.Broadcast(false, EndpointUsageMetricsArray);

        // Return
        return;
    }

    ///// VALID RESPONSE
    if (EHttpResponseCodes::IsOk(Response->GetResponseCode()))
    {
        // Get Response Content as String
        FString ResponseJson = Response->GetContentAsString();

        // Read response as JsonArray
        TArray<TSharedPtr<FJsonValue>> JsonArray;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseJson);

        ///// VALID JSON
        if (FJsonSerializer::Deserialize(Reader, JsonArray) && JsonArray.Num() > 0)
        {
            ///// LOOP JSONARRAY
            for (TSharedPtr<FJsonValue> JsonValue : JsonArray)
            {
                // Create JsonObject for current index
                TSharedPtr<FJsonObject> JsonObject = JsonValue->AsObject();

                // Create struct for output entry
                FEndpointUsageMetricsStruct EndpointUsageMetrics;

                // Get Response Info
                FString ServerTime = JsonObject->GetStringField("time");
                int32 Calls = JsonObject->GetIntegerField("calls");

                // Populate Struct
                EndpointUsageMetrics.Time = FCString::Atoi64(*ServerTime);
                EndpointUsageMetrics.Calls = JsonObject->GetIntegerField("calls");
                EndpointUsageMetrics.Endpoint = FName(JsonObject->GetStringField("endpoint"));

                // Add new entry to return array
                EndpointUsageMetricsArray.Add(EndpointUsageMetrics);

            }
            // Call Delegate when finished looping
            OnEndpointUsageMetrics.Broadcast(true, EndpointUsageMetricsArray);

            // Complete
            return;
        }

        ///// INVALID RESPONSE
        else
        {
            // Log
            UE_LOG(LogTemp, Error, TEXT("Invalid Response - Couldn't Deserialize Json Object"));

            // Call Delegate
            OnEndpointUsageMetrics.Broadcast(false, EndpointUsageMetricsArray);

            // Complete
            return;
        }
    }

    ///// INVALID RESPONSE
    else
    {
        // Log
        UE_LOG(LogTemp, Error, TEXT("Invalid Response - Is not Ok"));

        // Call Delegate
        OnEndpointUsageMetrics.Broadcast(false, EndpointUsageMetricsArray);

        // Complete
        return;
    }
}



/////
/////  CARDANO
/////////////////////////////
////////// 
/////