#include "MirrorCharacter.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"

AMirrorCharacter::AMirrorCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // Capsule setup
    GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

    // Movement
    GetCharacterMovement()->MaxWalkSpeed = 600.f;
    GetCharacterMovement()->bOrientRotationToMovement = false;

    // Rotation from controller
    bUseControllerRotationPitch = true;
    bUseControllerRotationYaw = true;
    bUseControllerRotationRoll = false;

    // Camera
    FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
    FirstPersonCameraComponent->SetRelativeLocation(FVector(0, 0, 64.f)); // Eye height
    FirstPersonCameraComponent->bUsePawnControlRotation = true;
}

void AMirrorCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        if (PC->PlayerCameraManager)
        {
            PC->PlayerCameraManager->ViewPitchMin = -80.f;
            PC->PlayerCameraManager->ViewPitchMax = 60.f;
        }

        if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
                ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
            {
                Subsystem->ClearAllMappings();
                Subsystem->AddMappingContext(DefaultMappingContext, 0);
                UE_LOG(LogTemp, Warning, TEXT("Input Mapping Context applied"));
            }
        }
    }
}

void AMirrorCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        Input->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &AMirrorCharacter::MoveForward);
        Input->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &AMirrorCharacter::MoveRight);
        Input->BindAction(TurnAction, ETriggerEvent::Triggered, this, &AMirrorCharacter::Turn);
        Input->BindAction(LookUpAction, ETriggerEvent::Triggered, this, &AMirrorCharacter::LookUp);
        Input->BindAction(InteractAction, ETriggerEvent::Started, this, &AMirrorCharacter::Interact);
        Input->BindAction(JumpAction, ETriggerEvent::Started, this, &AMirrorCharacter::Jump);
        Input->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMirrorCharacter::StopJumping);
    }
}

void AMirrorCharacter::MoveForward(const FInputActionValue& Value)
{
    float Input = Value.Get<float>();
    if (Controller && Input != 0.0f)
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FVector Direction = FRotationMatrix(FRotator(0, Rotation.Yaw, 0)).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Input);
    }
}

void AMirrorCharacter::MoveRight(const FInputActionValue& Value)
{
    float Input = Value.Get<float>();
    if (Controller && Input != 0.0f)
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FVector Direction = FRotationMatrix(FRotator(0, Rotation.Yaw, 0)).GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, Input);
    }
}

void AMirrorCharacter::Turn(const FInputActionValue& Value)
{
    float Input = Value.Get<float>();
    AddControllerYawInput(Input);
}

void AMirrorCharacter::LookUp(const FInputActionValue& Value)
{
    float Input = Value.Get<float>();
    AddControllerPitchInput(Input);
}

void AMirrorCharacter::Interact(const FInputActionValue& Value)
{
    UE_LOG(LogTemp, Warning, TEXT("Interact pressed"));
}
