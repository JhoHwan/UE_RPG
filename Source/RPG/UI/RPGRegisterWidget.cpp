// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RPGRegisterWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "RPGLoginLabel.h"
#include "RPGBlueprintFunctionLibrary.h"
#include "GameMode\RPGTitleGameMode.h"

#define SetFocus(Widget) URPGBlueprintFunctionLibrary::SetFocusToWidget(GetWorld(), Widget)

void URPGRegisterWidget::NativeConstruct()
{
	Super::NativeConstruct();

	check(EmailBox && PasswordBox && PasswordConfirmBox && ErrorLabel && RegisterBtn && Blocker);

	EmailBox->OnTextCommitted.AddDynamic(this, &URPGRegisterWidget::OnTextCommitted);
	PasswordBox->OnTextCommitted.AddDynamic(this, &URPGRegisterWidget::OnTextCommitted);
	PasswordConfirmBox->OnTextCommitted.AddDynamic(this, &URPGRegisterWidget::OnTextCommitted);
	RegisterBtn->OnClicked.AddDynamic(this, &URPGRegisterWidget::TryRegister);

	ErrorLabel->SetVisibility(ESlateVisibility::Collapsed);
	Blocker->SetVisibility(ESlateVisibility::Collapsed);
}

void URPGRegisterWidget::TryRegister()
{
	FString Email = EmailBox->GetText().ToString().TrimStartAndEnd();
	FString Password = PasswordBox->GetText().ToString().TrimStartAndEnd();
	FString ConfirmPassword = PasswordConfirmBox->GetText().ToString().TrimStartAndEnd();

	ErrorLabel->SetVisibility(ESlateVisibility::Collapsed);

	if (Email.IsEmpty() || Password.IsEmpty() || ConfirmPassword.IsEmpty())
	{
		PasswordBox->SetText(FText::GetEmpty());
		PasswordConfirmBox->SetText(FText::GetEmpty());
		SetFocus(EmailBox);

		ErrorLabel->SetErrorLabel(TEXT("Please fill in all fields."));
		return;
	}

	if (!URPGBlueprintFunctionLibrary::IsValidEmailFormat(Email))
	{
		PasswordBox->SetText(FText::GetEmpty());
		PasswordConfirmBox->SetText(FText::GetEmpty());
		SetFocus(EmailBox);

		ErrorLabel->SetErrorLabel(TEXT("Enter a valid email address."));
		return;
	}

	if (Password.Len() < 6)
	{
		PasswordBox->SetText(FText::GetEmpty());
		PasswordConfirmBox->SetText(FText::GetEmpty());
		SetFocus(PasswordBox);

		ErrorLabel->SetErrorLabel(TEXT("Password must be at least 6 characters long."));

		return;

	}

	if (Password != ConfirmPassword)
	{
		PasswordBox->SetText(FText::GetEmpty());
		PasswordConfirmBox->SetText(FText::GetEmpty());
		SetFocus(PasswordBox);

		ErrorLabel->SetErrorLabel(TEXT("Passwords do not match."));
		return;
	}

	ARPGTitleGameMode* TitleGameMode = CastChecked<ARPGTitleGameMode>(GetWorld()->GetAuthGameMode());

	Blocker->SetVisibility(ESlateVisibility::Visible);

	TitleGameMode->TryRegister(Email, Password, FOnRegisterResponse::CreateUObject(this, &URPGRegisterWidget::OnRegisterResponse));
}

void URPGRegisterWidget::OnRegisterResponse(bool bSuccess, const FString& Message)
{
	Blocker->SetVisibility(ESlateVisibility::Collapsed);

	PasswordBox->SetText(FText::GetEmpty());
	PasswordConfirmBox->SetText(FText::GetEmpty());

	if (!bSuccess)
	{
		SetFocus(EmailBox);

		ErrorLabel->SetErrorLabel(Message);
		return;
	}

	EmailBox->SetText(FText::GetEmpty());

	OnRegisterSuccess.Broadcast();
}

void URPGRegisterWidget::OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		TryRegister();
	}
}