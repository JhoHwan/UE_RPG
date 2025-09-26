// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGBlueprintFunctionLibrary.h"

FText URPGBlueprintFunctionLibrary::ConvertTextToEmailForm(const FText& InText, int32 maxLength)
{
    FString Input = InText.ToString();
    FString Filtered;

    int32 length = 0;

    for (TCHAR Char : Input)
    {
        if ((Char >= 'A' && Char <= 'Z') ||
            (Char >= 'a' && Char <= 'z') ||
            (Char >= '0' && Char <= '9') ||
            (Char == '@') || (Char == '.'))
        {
            ++length;
            Filtered.AppendChar(Char);
        }
        if (length >= maxLength) break;
    }

    return FText::FromString(Filtered);
}
