// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGLoginWidget.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Components/CheckBox.h"
#include "Components/Button.h"
#include "GameMode\RPGTitleGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGame/RPGLoginSaveGame.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "RPGBlueprintFunctionLibrary.h"
#include "RPGLoginLabel.h"

#define SetFocus(Widget) URPGBlueprintFunctionLibrary::SetFocusToWidget(GetWorld(), Widget)

void URPGLoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Validate widget bindings
	check(EmailBox && PasswordBox && LoginBtn && ErrorLabel && Blocker && SaveEmailCheckBox);

	EmailBox->OnTextCommitted.AddDynamic(this, &URPGLoginWidget::OnTextCommitted);
	PasswordBox->OnTextCommitted.AddDynamic(this, &URPGLoginWidget::OnTextCommitted);
	LoginBtn->OnClicked.AddDynamic(this, &URPGLoginWidget::TryLogin);

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

void URPGLoginWidget::TryLogin()
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

	if (!URPGBlueprintFunctionLibrary::IsValidEmailFormat(Email))
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

	ARPGTitleGameMode* TitleGameMode = CastChecked<ARPGTitleGameMode>(GetWorld()->GetAuthGameMode());

	Blocker->SetVisibility(ESlateVisibility::Visible);

	TitleGameMode->TryLogin(Email, Password, FOnLoginResponse::CreateUObject(this, &URPGLoginWidget::OnLoginResponse));
}

void URPGLoginWidget::OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		TryLogin();
	}
}

void URPGLoginWidget::OnLoginResponse(bool bSuccess, const FString& Message)
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

bool URPGLoginWidget::SaveLoginInfo(bool bRememberEmail, const FString& Email)
{
	URPGLoginSaveGame* LoginSaveGame = Cast<URPGLoginSaveGame>(UGameplayStatics::CreateSaveGameObject(URPGLoginSaveGame::StaticClass()));
	if (LoginSaveGame)
	{
		bCurRememberEmail = bRememberEmail;
		LoginSaveGame->bRememberEmail = bRememberEmail;
		LoginSaveGame->Email = Email;

		return UGameplayStatics::SaveGameToSlot(LoginSaveGame, URPGLoginSaveGame::SaveSlotName, 0);
	}

	return false;
}

bool URPGLoginWidget::LoadLoginInfo(FString& OutEmail, bool& OutRememberMe)
{
	URPGLoginSaveGame* LoadedLoginSaveGame = Cast<URPGLoginSaveGame>(UGameplayStatics::LoadGameFromSlot(URPGLoginSaveGame::SaveSlotName, 0));
	if (LoadedLoginSaveGame)
	{
		OutEmail = LoadedLoginSaveGame->Email;
		OutRememberMe = LoadedLoginSaveGame->bRememberEmail;
		return true;
	}

	return false;
}