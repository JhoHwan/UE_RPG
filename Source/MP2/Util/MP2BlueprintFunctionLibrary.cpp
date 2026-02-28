// Fill out your copyright notice in the Description page of Project Settings.


#include "MP2BlueprintFunctionLibrary.h"
#include "Engine/World.h"
#include "Components/Widget.h"

void UMP2BlueprintFunctionLibrary::SetFocusToWidget(const UObject* WorldContextObject, UWidget* Widget)
{
	if (!Widget) return;

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);

	if (World)
	{
		FTimerHandle Tmp;
		World->GetTimerManager().SetTimer(
			Tmp,
			FTimerDelegate::CreateWeakLambda(Widget, [Widget]()
				{
					Widget->SetKeyboardFocus();
				}),
			0.1f, false);
	}
}

bool UMP2BlueprintFunctionLibrary::IsValidEmailFormat(const FString& Email)
{
	static const FRegexPattern Pattern(TEXT("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$"));
	FRegexMatcher Matcher(Pattern, Email);
	return Matcher.FindNext();
}
