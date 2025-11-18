// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MP2LoginWidget.generated.h"

/**
 * 
 */

UCLASS()
class MP2_API UMP2LoginWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

protected:
	UFUNCTION()
	void TryLogin();

	UFUNCTION()
	void OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

private:
	void OnLoginResponse(bool bSuccess, const FString& Message);

	bool SaveLoginInfo(bool bRememberMe, const FString& Email);
	bool LoadLoginInfo(FString& OutEmail, bool& OutRememberMe);

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UEditableTextBox> EmailBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UEditableTextBox> PasswordBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UMP2LoginLabel> ErrorLabel;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UCheckBox> SaveEmailCheckBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> LoginBtn;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UWidget> Blocker;

private:
	bool bCurRememberEmail = false;
	bool bLoginPending = false;
};