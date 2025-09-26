// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGBlueprintFunctionLibrary.h"

FText URPGBlueprintFunctionLibrary::ConvertTextToEmailForm(const FText& InText)
{
    FString Input = InText.ToString();
    FString Filtered;

    for (TCHAR Char : Input)
    {
        if ((Char >= 'A' && Char <= 'Z') ||
            (Char >= 'a' && Char <= 'z') ||
            (Char >= '0' && Char <= '9') ||
            (Char == '@'))
        {
            Filtered.AppendChar(Char);
        }
    }

    return FText::FromString(Filtered);
}
