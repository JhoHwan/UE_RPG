// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGRegisterWidget.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRegisterSuccess);

UCLASS()
class RPG_API URPGRegisterWidget : public UUserWidget
{
	GENERATED_BODY()


protected:
	virtual void NativeConstruct() override;

protected:
	UFUNCTION()
	void TryRegister();
	void OnRegisterResponse(bool bSuccess, const FString& Message);

	UFUNCTION()
	void OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

public:
	UPROPERTY(BlueprintAssignable)
	FOnRegisterSuccess OnRegisterSuccess;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UEditableTextBox> EmailBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UEditableTextBox> PasswordBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UEditableTextBox> PasswordConfirmBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class URPGLoginLabel> ErrorLabel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> RegisterBtn;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UWidget> Blocker;
};
