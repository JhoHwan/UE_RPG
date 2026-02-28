// Fill out your copyright notice in the Description page of Project Settings.


#include "MP2LoginWidget.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Components/CheckBox.h"
#include "Components/Button.h"
#include "Core/Game/Login/MP2TitleGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Core/SaveGame/MP2LoginSaveGame.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Util/MP2BlueprintFunctionLibrary.h"
#include "Control/MP2LoginLabel.h"

#define SetFocus(Widget) UMP2BlueprintFunctionLibrary::SetFocusToWidget(GetWorld(), Widget)

void UMP2LoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Validate widget bindings
	check(EmailBox && PasswordBox && LoginBtn && ErrorLabel && Blocker && SaveEmailCheckBox);

	EmailBox->OnTextCommitted.AddDynamic(this, &UMP2LoginWidget::OnTextCommitted);
	PasswordBox->OnTextCommitted.AddDynamic(this, &UMP2LoginWidget::OnTextCommitted);
	LoginBtn->OnClicked.AddDynamic(this, &UMP2LoginWidget::TryLogin);

	ErrorLabel->SetVisibility(ESlateVisibility::Collapsed);
	Blocker->SetVisibility(ESlateVisibility::Collapsed);

	FString Email;
	bool bRememberEmail;

	if (!LoadLoginInfo(Email, bRememberEmail))
	{
		SaveLoginInfo(false, TEXT(""));
	}

	if (bRememberEmail)
	{
		bCurRememberEmail = true;
		EmailBox->SetText(FText::FromString(Email));

		SaveEmailCheckBox->SetIsChecked(true);
	}

	// Set initial keyboard focus
	if (Email.IsEmpty()) { EmailBox->SetKeyboardFocus(); }
	else { PasswordBox->SetKeyboardFocus(); }
}

void UMP2LoginWidget::TryLogin()
{
	if (bLoginPending) return;
	ErrorLabel->SetVisibility(ESlateVisibility::Collapsed);

	FString Email = EmailBox->GetText().ToString().TrimStartAndEnd();
	FString Password = PasswordBox->GetText().ToString().TrimStartAndEnd();

	if (bCurRememberEmail && !SaveEmailCheckBox->IsChecked())
	{
		SaveLoginInfo(false, TEXT(""));
	}

	//Email and Password validation check
	if (Email.IsEmpty() || Password.IsEmpty())
	{
		SetFocus(EmailBox);

		ErrorLabel->SetErrorLabel(TEXT("Email or Password cannot be empty."));

		return;
	}

	if (!UMP2BlueprintFunctionLibrary::IsValidEmailFormat(Email))
	{
		PasswordBox->SetText(FText::GetEmpty());
		SetFocus(EmailBox);


		ErrorLabel->SetErrorLabel(TEXT("Invalid email format."));

		return;
	}

	if(Password.Len() < 6)
	{
		PasswordBox->SetText(FText::GetEmpty());
		SetFocus(PasswordBox);


		ErrorLabel->SetErrorLabel(TEXT("Password must be at least 6 characters long."));
		return;
	}

	AMP2TitleGameMode* TitleGameMode = CastChecked<AMP2TitleGameMode>(GetWorld()->GetAuthGameMode());

	Blocker->SetVisibility(ESlateVisibility::Visible);

	//TitleGameMode->TryLogin(Email, Password, FOnLoginResponse::CreateUObject(this, &UMP2LoginWidget::OnLoginResponse));
}

void UMP2LoginWidget::OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		TryLogin();
	}
}

void UMP2LoginWidget::OnLoginResponse(bool bSuccess, const FString& Message)
{
	if (!bSuccess)
	{
		Blocker->SetVisibility(ESlateVisibility::Collapsed);

		PasswordBox->SetText(FText::GetEmpty());
		SetFocus(PasswordBox);

		ErrorLabel->SetErrorLabel(Message);

		return;
	}

	if(SaveEmailCheckBox && SaveEmailCheckBox->IsChecked())
	{
		bool bSaveSuccess = SaveLoginInfo(true, EmailBox->GetText().ToString());
		if (!bSaveSuccess)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to save login info."));
		}
	}

	// TODO : Open Character Selection Widget

	return;
}

bool UMP2LoginWidget::SaveLoginInfo(bool bRememberEmail, const FString& Email)
{
	UMP2LoginSaveGame* LoginSaveGame = Cast<UMP2LoginSaveGame>(UGameplayStatics::CreateSaveGameObject(UMP2LoginSaveGame::StaticClass()));
	if (LoginSaveGame)
	{
		bCurRememberEmail = bRememberEmail;
		LoginSaveGame->bRemember = bRememberEmail;
		LoginSaveGame->Email = Email;

		return UGameplayStatics::SaveGameToSlot(LoginSaveGame, UMP2LoginSaveGame::SaveSlotName, 0);
	}

	return false;
}

bool UMP2LoginWidget::LoadLoginInfo(FString& OutEmail, bool& OutRememberMe)
{
	UMP2LoginSaveGame* LoadedLoginSaveGame = Cast<UMP2LoginSaveGame>(UGameplayStatics::LoadGameFromSlot(UMP2LoginSaveGame::SaveSlotName, 0));
	if (LoadedLoginSaveGame)
	{
		OutEmail = LoadedLoginSaveGame->Email;
		OutRememberMe = LoadedLoginSaveGame->bRemember;
		return true;
	}

	return false;
}