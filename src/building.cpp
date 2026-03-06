#include "building.h"
#include <iostream>

void FactoryBuilding::UpdateBuilding(float dt)
{
	if (status == BuildingStatus::Unpowered || selectedRecipe == -1)
	{
		return;
	}

	if (inputInventoryChanged == true)
	{
		// Use recipe.inputItems (vector<ItemQuantity>) instead of old inputs array
		const Recipe& recipe = recipes[selectedRecipe];
		for (const ItemQuantity& req : recipe.inputItems)
		{
			ItemType reqType = static_cast<ItemType>(req.item.itemID);
			if (req.quantity > inputInventory.GetItem(reqType)) // see if we have enough items to craft
			{
				status = BuildingStatus::Idle;
				inputInventoryChanged = false;
				craftTimer = 0;
				return;
			}
		}
		status = BuildingStatus::Working;
		inputInventoryChanged = false;
	}

	if (status != BuildingStatus::Idle)
	{
		craftTimer += dt;
		status = BuildingStatus::Working;
		if (craftTimer >= recipes[selectedRecipe].recipeDuration)
		{
			craftTimer = 0;
			ResolveRecipe(&recipes[selectedRecipe]);
		}
	}
}

void FactoryBuilding::SelectRecipe(int recipeNum)
{
	if (!(recipeNum >= 0 && recipeNum <= (int)recipes.size())) //check if selecting valid recipe
	{
		return; //out of bounds
	}
	
	inputInventoryChanged = true;
	selectedRecipe = recipeNum;
}

bool FactoryBuilding::InputItems(ItemType item, int count)
{
	inputInventoryChanged = true;
	inputInventory.AddItem(item, count);
	return true;
}

void FactoryBuilding::ResolveRecipe(Recipe* recipe)
{
	// Consume inputs and add outputs
	inputInventoryChanged = true;

	// Consume inputs
	for (const ItemQuantity& in : recipe->inputItems)
	{
		ItemType t = static_cast<ItemType>(in.item.itemID);
		// best-effort: remove what we can (should be available if we started crafting)
		inputInventory.RemoveItem(t, in.quantity);
	}

	// Add outputs to the building's outputInventory
	for (const ItemQuantity& out : recipe->outputItems)
	{
		ItemType t = static_cast<ItemType>(out.item.itemID);
		outputInventory.AddItem(t, out.quantity);
	}
}

FactoryBuilding::FactoryBuilding(std::vector<Recipe> recipes_, int selectedRecipe_)
{
	recipes = recipes_;

	SelectRecipe(selectedRecipe_);
}

FactoryBuilding::FactoryBuilding(const FactoryBuilding& other)
{
	status = other.status;
	recipes = other.recipes;
	selectedRecipe = other.selectedRecipe;

	inputs = other.inputs;
	outputs = other.outputs;
	inputInventory = other.inputInventory;
	outputInventory = other.outputInventory;

	craftTimer = 0;
	inputInventoryChanged = true;
}

FactoryBuilding::FactoryBuilding()
{

}

void PowerGrid::UpdateBuilding(float dt)
{
	for (int i = 0; i < connectedSources.size(); i++)
	{
		if (connectedSources[i].status == BuildingStatus::Working)
		{
			Recipe curRecipe = connectedSources[i].recipes[connectedSources[i].selectedRecipe];
			totalGenerated += curRecipe.powerDraw;
		}
	}
	for (int i = 0; i < connectedSinks.size(); i++)
	{
		if (connectedSinks[i].status == BuildingStatus::Working)
		{
			Recipe curRecipe = connectedSinks[i].recipes[connectedSinks[i].selectedRecipe];
			totalUseage += curRecipe.powerDraw;
		}
	}
	
	if (totalUseage > totalGenerated) //not enough power, crash grid
	{
		for (int i = 0; i < connectedSources.size(); i++)
		{
			connectedSources[i].status = BuildingStatus::Unpowered;
		}
		for (int i = 0; i < connectedSinks.size(); i++)
		{
			connectedSinks[i].status = BuildingStatus::Unpowered;
		}
	}
}

void PowerGrid::StartGrid()
{
	for (int i = 0; i < connectedSources.size(); i++)
	{
		connectedSources[i].status = BuildingStatus::Idle;
	}
	for (int i = 0; i < connectedSinks.size(); i++)
	{
		connectedSinks[i].status = BuildingStatus::Idle;
	}
}