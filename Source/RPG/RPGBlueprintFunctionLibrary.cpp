// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGBlueprintFunctionLibrary.h"
#include "Engine/World.h"
#include "Components/Widget.h"

void URPGBlueprintFunctionLibrary::SetFocusToWidget(UWorld* World, UWidget* Widget)
{
	if (World)
	{
		FTimerHandle Tmp;
		World->GetTimerManager().SetTimer(
			Tmp,
			FTimerDelegate::CreateWeakLambda(Widget, [Widget]()
				{
					Widget->SetKeyboardFocus();
				}),
			0.2f, false);
	}
}

bool URPGBlueprintFunctionLibrary::IsValidEmailFormat(const FString& Email)
{
	static const FRegexPattern Pattern(TEXT("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$"));
	FRegexMatcher Matcher(Pattern, Email);
	return Matcher.FindNext();
}
