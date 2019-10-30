#pragma once
namespace Reality
{
	struct ModelComponent
	{		
		// Path to mesh
		std::string mesh;
		// Model ID
		int modelId;

		//// Path to shader
		//std::string vertexShader;
		//std::string fragmentShader;

		// Constructor
		ModelComponent(std::string const _model = "") : mesh(_model)
		{
			modelId = -1;
		}
	};
}
