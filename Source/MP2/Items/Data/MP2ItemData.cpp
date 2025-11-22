// Fill out your copyright notice in the Description page of Project Settings.


#include "MP2ItemData.h"
#include "Misc\DataValidation.h"

#if WITH_EDITOR
EDataValidationResult UMP2ItemData::IsDataValid(FDataValidationContext& Context) const
{
    if (ItemName.IsEmpty())
    {
        Context.AddError(FText::FromString(TEXT("ItemName cannot be empty.")));
    }

    return Context.GetNumErrors() > 0 ? EDataValidationResult::Invalid : EDataValidationResult::Valid;
}
#endif