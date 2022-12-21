////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////
/////
///// blockfrost integration - Originally built by VenomStyx (https://github.com/VenomStyx)
/////
/////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////
////////// 
/////



#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Http.h" 
#include "blockfrost_structs.h"
#include "blockfrost_subsystem.generated.h"



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////
////////// 
/////



/////
///// EVENT DELEGATES
/////////////////////////////
////////// 
/////


	/////
	///// BLOCKFROST
	/////////////////////////////
	////////// 
	/////

	///// ROOT ENDPOINT
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRootEndpointDelegate, bool, Success, FString, RootEndpoint);

	///// BACKEND STATUS
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBackendStatusDelegate, bool, Success, bool, Status);

	///// BACKEND TIME
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBackendTimeDelegate, bool, Success, int64, Time);

	///// USAGE METRICS
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUsageMetricsDelegate, bool, Success, const TArray<FUsageMetricsStruct>&, UsageMetrics);

	///// ENDPOINT USAGE METRICS
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEndpointUsageMetricsDelegate, bool, Success, const TArray<FEndpointUsageMetricsStruct>&, EndpointUsageMetrics);



UCLASS() 
class BLOCKFROST_API Ublockfrost_subsystem : public UGameInstanceSubsystem
{GENERATED_BODY()
//////////
///// PUBLIC
public:


	/////
	///// SUBSYSTEM
	/////////////////////////////
	////////// 
	/////

	// MAKE BLOCKFROST REQUEST
	////////////////////////////////////////
	// Creates a basic request for usage with the blockfrost subsystem. 
	TSharedRef<IHttpRequest> MakeBlockfrostRequest(const FName& ProjectKey, const FString& RequestVerb, const FString& RequestURL);

	bool GetValidJsonArray(FHttpResponsePtr Response, TSharedRef<TJsonReader<>>& OutReader, TArray<TSharedPtr<FJsonValue>>& OutJsonArray);
	bool GetValidJsonObject(FHttpResponsePtr Response, TSharedRef<TJsonReader<>>& OutReader, TSharedPtr<FJsonObject> OutJsonObject);


	/////
	///// BLOCKFROST
	/////////////////////////////
	////////// 
	/////


	// ROOT ENDPOINT 
	////////////////////////////////////////
	// Request Root Endpoint - has no other function than to point end users to documentation
	UFUNCTION(BlueprintCallable, Category = "blockfrost|Network|Server")
	void RequestRootEndpoint(const FName ProjectName);

	// Delegate for Root Endpoint
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "blockfrost|Network|Server")
	FRootEndpointDelegate OnRootEndpoint;


	// BACKEND STATUS 
	////////////////////////////////////////
	// Request Return Backend Status - as a boolean. Your application should handle situations when backend for the given chain is unavailable
	UFUNCTION(BlueprintCallable, Category = "blockfrost|Network|Server")
	void RequestBackendStatus(const FName ProjectName);

	// Delegate for Backend Status
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "blockfrost|Network|Server")
	FBackendStatusDelegate OnBackendStatus;



	// BACKEND TIME
	////////////////////////////////////////
	// This endpoint provides the current UNIX time. Your aplpication might use this to verify if the client clock is not out of sync. 
	UFUNCTION(BlueprintCallable, Category = "blockfrost|Network|Server")
	void RequestBackendTime(const FName ProjectName);

	// Delegate for Backend Time	
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "blockfrost|Network|Server")
	FBackendTimeDelegate OnBackendTime;
	


	// USAGE METRICS
	////////////////////////////////////////
	// History of your blockfrost usage metrics in the past 30 days. 
	UFUNCTION(BlueprintCallable, Category = "blockfrost|Network|Metrics")
	void RequestUsageMetrics(const FName ProjectName);

	// Delegate for Usage Metrics
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "blockfrost|Network|Metrics")
	FUsageMetricsDelegate OnUsageMetrics;



	// ENDPOINT USAGE METRICS
	////////////////////////////////////////
	// History of your blockfrost usage metrics per endpoint in the past 30 days. 
	UFUNCTION(BlueprintCallable, Category = "blockfrost|Network|Metrics")
	void RequestEndpointUsageMetrics(const FName ProjectName);

	// Delegate for Endpoint Usage Metrics
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "blockfrost|Network|Metrics")
	FEndpointUsageMetricsDelegate OnEndpointUsageMetrics;



//////////
///// PROTECTED
protected:


	///// ON INIT GAMEINSTANCE
	void Initialize(FSubsystemCollectionBase& Collection) override;

	

//////////
///// PRIVATE
private:
	

	/////
	///// SUBSYSTEM
	/////////////////////////////
	////////// 
	/////

	// PROJECT ID
	////////////////////////////////////////
	// Authorizes your Requests with the blockfrost API
	UPROPERTY(VisibleAnywhere, Transient, Category = Input)
	FString ProjectID;


	/////
	///// BLOCKFROST
	/////////////////////////////
	////////// 
	/////


	// ROOT ENDPOINT 
	////////////////////////////////////////
	// Receive Event for Root Endpoint
	void ReceiveRootEndpoint(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);


	// BACKEND STATUS 
	////////////////////////////////////////
	// Receive Event for Backend Status
	void ReceiveBackendStatus(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful); 


	// BACKEND TIME
	////////////////////////////////////////
	// Receive Event for Backend Time
	void ReceiveBackendTime(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);


	// USAGE METRICS
	////////////////////////////////////////
	// Receive Event for Usage Metrics
	void ReceiveUsageMetrics(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful); 


	// ENDPOINT USAGE METRICS
	////////////////////////////////////////
	// Receive Event for Endpoint Usage Metrics
	void ReceiveEndpointUsageMetrics(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful); 

};

