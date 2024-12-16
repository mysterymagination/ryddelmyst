// Copyright Jeff Creswell 2024. Subject to the GPL v3.0 license.


#include "LambdaButton.h"

void void ULambdaButton::BindUniqueLambda(TFunctionRef<void ()> Lambda)
{
    if (LambdaEvent.IsBound())
    {
        LambdaEvent.UnBind();
    }
    LambdaEvent.BindLambda(Lambda);
}

void ULambdaButton::ExecLambda()
{
    LambdaEvent.ExecuteIfBound();
}