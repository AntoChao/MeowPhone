#include "MPEnvActorPushable.h"

#include "../../CommonStruct.h"
#include "MPFractureEnvActor.h"

AMPEnvActorPushable::AMPEnvActorPushable()
{
	envActorTag = EEnvActor::EPushable;
	envActorType = EEnvActorType::EDirectInteract;

	PrimaryActorTick.bCanEverTick = false;

    envActorBodyMesh->SetSimulatePhysics(false);
    envActorBodyMesh->SetNotifyRigidBodyCollision(true); // To receive OnHit
}

void AMPEnvActorPushable::BeginPlay()
{
    Super::BeginPlay();

    envActorBodyMesh->OnComponentHit.AddDynamic(this, &AMPEnvActorPushable::OnHit);
}

bool AMPEnvActorPushable::IsInteractable(AMPCharacter* targetActor)
{
	return true;
}

FText AMPEnvActorPushable::GetInteractHintText(AMPCharacter* targetActor)
{
	return envActorName;
}

void AMPEnvActorPushable::ApplyInteractEffectDirect(AMPCharacter* targetActor)
{
    if (!targetActor || isAlreadyPushed) return;

    FVector pushDrection = (GetActorLocation() - targetActor->GetActorLocation()).GetSafeNormal();

    // Start simulating physics and push
    envActorBodyMesh->SetSimulatePhysics(true);
    envActorBodyMesh->AddImpulse(pushDrection * pushForce, NAME_None, true);

    isAlreadyPushed = true;
}

void AMPEnvActorPushable::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    float impactForce = NormalImpulse.Size();

    UE_LOG(LogTemp, Warning, TEXT("Impact force: %f"), impactForce);

    if (bIsBreakable && impactForce >= BreakableThreshold)
    {
        UE_LOG(LogTemp, Warning, TEXT("Object broke due to impact!"));

        if (FracturedVersionClass)
        {
            AMPFractureEnvActor* fractureActor = GetWorld()->SpawnActor<AMPFractureEnvActor>(FracturedVersionClass, GetActorTransform());
			if (fractureActor)
			{
				fractureActor->ApplyForce(NormalImpulse);
			}
        }

        Destroy();
    }
}