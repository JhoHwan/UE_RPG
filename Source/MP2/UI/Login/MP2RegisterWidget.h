// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MP2RegisterWidget.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRegisterSuccess);

UCLASS()
class MP2_API UMP2RegisterWidget : public UUserWidget
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
	TObjectPtr<class UMP2LoginLabel> ErrorLabel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> RegisterBtn;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UWidget> Blocker;
};
