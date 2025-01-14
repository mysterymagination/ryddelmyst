// Copyright Jeff Creswell 2024. Subject to the GPL v3.0 license.


#include "LambdaButton.h"

void ULambdaButton::ExecLambda()
{
    LambdaEvent.ExecuteIfBound();
}