// Copyright 2015-2019 Piperift. All Rights Reserved.

#include "LifetimeComponent.h"
#include "Serialization/MTTask.h"


ULifetimeComponent::ULifetimeComponent()
	: Super()
{}

void ULifetimeComponent::BeginPlay()
{
	Super::BeginPlay();

	if (USaveManager* Manager = GetManager())
	{
		Manager->SubscribeForEvents(this);

		// If manager is loading, it has probably manually created
		// this actor and its not a natural start
		if (!Manager->IsLoading())
		{
			Start.Broadcast();
		}
	}
	else
	{
		UE_LOG(LogSaveExtension, Error, TEXT("LifetimeComponent couldnt find a SaveManager. It will do nothing."))
	}
}

void ULifetimeComponent::EndPlay(EEndPlayReason::Type Reason)
{
	if (USaveManager* Manager = GetManager())
	{
		// If manager is loading, it has probably manually destroyed
		// this actor and its not a natural destroy
		if (!Manager->IsLoading())
		{
			Finish.Broadcast();
		}

		Manager->UnsubscribeFromEvents(this);
	}

	Super::EndPlay(Reason);
}

void ULifetimeComponent::OnSaveBegan(const FSaveFilter& Filter)
{
	if (Filter.ShouldSave(GetOwner()))
	{
		Saved.Broadcast();
	}
}

void ULifetimeComponent::OnLoadFinished(const FSaveFilter& Filter, bool bError)
{
	if (Filter.ShouldSave(GetOwner()))
	{
		Resume.Broadcast();
	}
}
