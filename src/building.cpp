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
		for (int i = 0; i < inputInventory.MAX_ITEMS; i++) //loop through inventory
		{
			if (recipes[selectedRecipe].inputs.quantities[i] > inputInventory.quantities[i]) //see if we have enough items to craft
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
	if (!(recipeNum >= 0 && recipeNum <= recipes.size())) //check if selecting valid recipe
	{
		return; //out of bounds
	}
	

	//output items inside into player inv if any inside
	//empty all items in building

	inputInventoryChanged = true;
	selectedRecipe = recipeNum;
}

bool FactoryBuilding::InputItems(ItemType item, int count)
{
	inputInventoryChanged = true;
	inputInventory.AddItem(item, count);
}

void FactoryBuilding::ResolveRecipe(Recipe* recipe)
{
	inputInventoryChanged = true;
	for (int i = 0; i < recipe->inputs.MAX_ITEMS; i++)
	{
		inputInventory.AddItem((ItemType)i, recipe->inputs.quantities[i]);
	}
	for (int i = 0; i < recipe->outputs.MAX_ITEMS; i++)
	{
		inputInventory.AddItem((ItemType)i, recipe->outputs.quantities[i]);
	}
	//std::cout << "Factory Output: " << outputInventory[0].quantity << " " << recipe->outputItems[0].item.name;
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