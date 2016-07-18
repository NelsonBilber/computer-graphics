// Fill out your copyright notice in the Description page of Project Settings.

#include "BuildingRoom.h"
#include "Grabber.h"


// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

    FindPhysicsHandleComponent();
    
    SetupInputComponent();
	
}

void UGrabber::FindPhysicsHandleComponent(){
    
    UE_LOG(LogTemp, Warning, TEXT("Grabber reporting for dutty !"));
    
    PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
    if(PhysicsHandle)
    {
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("component fail  %s  to deal with physics handle"),
               *(GetOwner()->GetName()) );
        
    }

}

void UGrabber::SetupInputComponent(){
   
    InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
    if(InputComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("Input component found !"));
        InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
        InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
        
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("component fail  %s  to deal with input component"),
               *(GetOwner()->GetName()) );
        
    }
}

void UGrabber::Grab(){
    
    UE_LOG(LogTemp, Warning, TEXT("Grab !"));
    
    GetFirstPhysicsBodyInReach();
    
}

void UGrabber::Release(){
    
    UE_LOG(LogTemp, Warning, TEXT("Release !"));
    
}



// Called every frame
void UGrabber::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	

}


 const FHitResult UGrabber::GetFirstPhysicsBodyInReach(){
     // GET PLAYER VIEW POINT THIS TICK
     
     auto World = GetWorld();
     if(World){
         FVector PlayerViewPointLocation;
         FRotator PlayerViewPointRotation;
         World->GetFirstPlayerController()->GetPlayerViewPoint(PlayerViewPointLocation, PlayerViewPointRotation );
         
         //UE_LOG(LogTemp, Warning, TEXT("location %s , position %s "), *PlayerViewPointLocation.ToString(),*PlayerViewPointRotation.ToString());
         
         FVector LineTraceEnd = PlayerViewPointLocation + ( PlayerViewPointRotation.Vector() * Reach );
         
         DrawDebugLine(
                       World,
                       PlayerViewPointLocation,
                       LineTraceEnd,
                       FColor::Red,
                       false,
                       0.0f,
                       0.0f,
                       10.0f
                       
                       );
         
         // (line trace)ray-cast out to reach distance
         
         FHitResult HitResult;
         FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
         GetWorld()->LineTraceSingleByObjectType(
                                                 HitResult,
                                                 PlayerViewPointLocation,
                                                 LineTraceEnd,
                                                 FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
                                                 TraceParameters
                                                 );
         AActor* ActorHit = HitResult.GetActor();
         if(ActorHit){
             UE_LOG(LogTemp, Warning, TEXT("line trace hit+: %s "), *(ActorHit->GetName()));
         }
         
     }
     
     return FHitResult();
 }
