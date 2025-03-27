#include "MPEnvActorCompPushable.h"

#include "../../CommonStruct.h"
#include "MPEnvActorCompFracture.h"

AMPEnvActorCompPushable::AMPEnvActorCompPushable()
{
	envActorTag = EEnvActor::EPushable;
	envActorType = EEnvActorType::EDirectInteract;

	PrimaryActorTick.bCanEverTick = false;

    envActorBodyMesh->SetSimulatePhysics(false);
    envActorBodyMesh->SetNotifyRigidBodyCollision(true); // To receive OnHit
}

void AMPEnvActorCompPushable::BeginPlay()
{
    Super::BeginPlay();

    if (envActorBodyMesh)
    {
        envActorBodyMesh->OnComponentHit.AddDynamic(this, &AMPEnvActorCompPushable::OnHit);
    }
}

bool AMPEnvActorCompPushable::IsInteractable(AMPCharacter* targetActor)
{
	return true;
}

FText AMPEnvActorCompPushable::GetInteractHintText(AMPCharacter* targetActor)
{
	return envActorName;
}

void AMPEnvActorCompPushable::ApplyInteractEffectDirect(AMPCharacter* targetActor)
{
    GEngine->AddOnScreenDebugMessage(7, 5.0f, FColor::Yellow, TEXT("Pushable EnvActor Applying Direct Effect"));

    if (!targetActor || isAlreadyPushed) return;

    FVector pushDrection = (GetActorLocation() - targetActor->GetActorLocation()).GetSafeNormal();

    // Start simulating physics and push
    envActorBodyMesh->SetSimulatePhysics(true);
    envActorBodyMesh->AddImpulse(pushDrection * pushForce, NAME_None, true);

    isAlreadyPushed = true;
}

void AMPEnvActorCompPushable::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    GEngine->AddOnScreenDebugMessage(7, 5.0f, FColor::Yellow, TEXT("Pushable EnvActor OnHIT called"));

    float impactForce = NormalImpulse.Size();

    if (isBreakable && impactForce >= breakableThreshold)
    {
        GEngine->AddOnScreenDebugMessage(7, 5.0f, FColor::Yellow, TEXT("Object broke due to impact!"));

        if (fracturedItemClass)
        {
            AMPEnvActorCompFracture* fractureActor = GetWorld()->SpawnActor<AMPEnvActorCompFracture>(fracturedItemClass, GetActorTransform());
			if (fractureActor)
			{
				fractureActor->ApplyForce(NormalImpulse);
			}
        }

        Destroy();
    }
}