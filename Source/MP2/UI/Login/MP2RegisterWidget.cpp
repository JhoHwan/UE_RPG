// Fill out your copyright notice in the Description page of Project Settings.


#include "MP2RegisterWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Control/MP2LoginLabel.h"
#include "Util/MP2BlueprintFunctionLibrary.h"
#include "Core/Game/Login/MP2TitleGameMode.h"

#define SetFocus(Widget) UMP2BlueprintFunctionLibrary::SetFocusToWidget(GetWorld(), Widget)

void UMP2RegisterWidget::NativeConstruct()
{
	Super::NativeConstruct();

	check(EmailBox && PasswordBox && PasswordConfirmBox && ErrorLabel && RegisterBtn && Blocker);

	EmailBox->OnTextCommitted.AddDynamic(this, &UMP2RegisterWidget::OnTextCommitted);
	PasswordBox->OnTextCommitted.AddDynamic(this, &UMP2RegisterWidget::OnTextCommitted);
	PasswordConfirmBox->OnTextCommitted.AddDynamic(this, &UMP2RegisterWidget::OnTextCommitted);
	RegisterBtn->OnClicked.AddDynamic(this, &UMP2RegisterWidget::TryRegister);

	ErrorLabel->SetVisibility(ESlateVisibility::Collapsed);
	Blocker->SetVisibility(ESlateVisibility::Collapsed);
}

void UMP2RegisterWidget::TryRegister()
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

	if (!UMP2BlueprintFunctionLibrary::IsValidEmailFormat(Email))
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

	AMP2TitleGameMode* TitleGameMode = CastChecked<AMP2TitleGameMode>(GetWorld()->GetAuthGameMode());

	Blocker->SetVisibility(ESlateVisibility::Visible);

	//TitleGameMode->TryRegister(Email, Password, FOnRegisterResponse::CreateUObject(this, &UMP2RegisterWidget::OnRegisterResponse));
}

void UMP2RegisterWidget::OnRegisterResponse(bool bSuccess, const FString& Message)
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

void UMP2RegisterWidget::OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		TryRegister();
	}
}