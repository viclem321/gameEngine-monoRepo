
        // CLEAR

		glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // END CLEAR





        // DRAWS

		map1.Draw(shaderModelMat);

		for(int i=0; i<InfoGlobal::nMaxClients; i++)
		{
			if(allPlayers[i]->connected == true)
			{
				if(i == character.nClient)
				{
					character.Draw(shaderPerso);
				}
				else
				{
					allPlayers[i]->Draw(shaderPerso);
				}
			}
		}






		//test
		/*for(int i = 0; i < 10000; i++)
		{
			allArbres[i]->Draw(shaderModel);
		}*/
		for(int i = 0; i < 13; i++)
		{
			if(allTree1[i]->isOnFrustum(camera)) allTree1[i]->Draw(shaderModelVec);
		}
		
		for(int i = 0; i < 5; i++)
		{
			if(allTree2[i]->isOnFrustum(camera)) allTree2[i]->Draw(shaderModelVec);
		}

		for(int i = 0; i < 8; i++)
		{
			if(allTree3[i]->isOnFrustum(camera)) allTree3[i]->Draw(shaderModelVec);
		}

		for(int i = 0; i < 13; i++)
		{
			if(allTree4[i]->isOnFrustum(camera)) allTree4[i]->Draw(shaderModelVec);
		}

		for(int i = 0; i < 18; i++)
		{
			if(allTree5[i]->isOnFrustum(camera)) allTree5[i]->Draw(shaderModelVec);
		}

		for(int i = 0; i < 59; i++)
		{
			if(allTree6[i]->isOnFrustum(camera)) allTree6[i]->Draw(shaderModelVec);
		}

		if(house1->isOnFrustum(camera)) house1->Draw(shaderModelVec);

		if(rock1->isOnFrustum(camera)) rock1->Draw(shaderModelVec);



		// END DRAW




		// SWAP BUFFER GLFW
		glfwSwapBuffers(window);