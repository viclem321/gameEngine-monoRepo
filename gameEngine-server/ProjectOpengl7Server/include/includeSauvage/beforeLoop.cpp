
	//-------------PREPARATION BEFORE LOOP-----------------
	//(trop de donnes dans la pile, a voir plus tard)

	//load map
	Map map1("ressources/maps/heightMapTerrain4.png", "map1", 960, 130);




	Model ourModelTree1(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(6.f, 10.f, 9.0f), 'N');
	glm::vec3 allModelsTree1[13] = {glm::vec3(86.7284, 3.05312, 46.1192), glm::vec3(122.516, 14.6828, 218.281), glm::vec3(99.0414, 5.8148, 108.301), glm::vec3(66.503, 5.9239, 353.566), glm::vec3(224.238, 19.3656, 624.877), glm::vec3(409.612, 12.1363, 877.542), glm::vec3(596.661, 49.9307, 592.907), glm::vec3(716.244, 17.0586, 813.805), glm::vec3(876.943, 9.1212, 563.385), glm::vec3(780.92, 34.5631, 270.504), glm::vec3(480.171, 39.5583, 270.504), glm::vec3(243.206, 39.316, 156.223), glm::vec3(480.171, 48.1753, 485.771) };
	ObjectInWorld* allTree1[13];
	for(int i = 0; i < 13; i++)
	{
		allTree1[i] = new ObjectInWorld(&ourModelTree1, &allModelsTree1[i], 'N', false, 'a');
	}


	Model ourModelTree2(glm::vec3(0.0f, 21.0f, 0.0f), glm::vec3(24.f, 21.f, 20.0f), 'N');
	glm::vec3 allModelsTree2[5] = {glm::vec3(770.81f, 24.27f, 150.87f), glm::vec3(659.184f, 37.99f, 473.626f), glm::vec3(842.267f, 4.375f, 887.53f), glm::vec3(432.32f, 47.395f, 567.778f), glm::vec3(217.288, 10.8672, 804.236)};
	ObjectInWorld* allTree2[5];
	for(int i = 0; i < 5; i++)
	{
		allTree2[i] = new ObjectInWorld(&ourModelTree2, &allModelsTree2[i], 'N', false, 'a');
	}


	Model ourModelTree3(glm::vec3(0.0f, 20.0f, 0.0f), glm::vec3(8.f, 20.f, 8.0f), 'N');
	glm::vec3 allModelsTree3[8] = {glm::vec3(228.23, 7.8879, 59.1855), glm::vec3(310.794, 34.1889, 421.112), glm::vec3(405.198, 18.5651, 828.394), glm::vec3(109.901, 7.1982, 828.394), glm::vec3(54.4794, 4.7983, 763.813), glm::vec3(188.295, 8.6713, 725.948), glm::vec3(99.1494, 8.4173, 639.791), glm::vec3(54.719, 3.8196, 925.88) };
	ObjectInWorld* allTree3[8];
	for(int i = 0; i < 8; i++)
	{
		allTree3[i] = new ObjectInWorld(&ourModelTree3, &allModelsTree3[i], 'N', false, 'a');
	}


	Model ourModelTree4(glm::vec3(0.0f, 12.0f, 0.0f), glm::vec3(6.f, 12.f, 6.0f), 'N');
	glm::vec3 allModelsTree4[13] = { glm::vec3(88.3394, 3.9712, 886.026), glm::vec3(59.1209, 6.683, 582.443), glm::vec3(171.187, 18.5195, 545.122), glm::vec3(149.395, 11.9957, 628.704), glm::vec3(241.729, 17.9691, 663.635), glm::vec3(117.484, 9.0897, 716.346), glm::vec3(135.483, 5.361, 878.738), glm::vec3(213.333, 4.9713, 902.991), glm::vec3(289.971, 22.7561, 800.214), glm::vec3(291.731, 10.1018, 889.954), glm::vec3(438.232, 43.6958, 603.497), glm::vec3(583.346, 55.5362, 781.937), glm::vec3(472.874, 38.0329, 726.368) };
	ObjectInWorld* allTree4[13];
	for(int i = 0; i < 13; i++)
	{
		allTree4[i] = new ObjectInWorld(&ourModelTree4, &allModelsTree4[i], 'N', false, 'a');
	}


	Model ourModelTree5(glm::vec3(0.0f, 8.0f, 0.0f), glm::vec3(3.f, 8.f, 3.0f), 'N');
	glm::vec3 allModelsTree5[18] = { glm::vec3(51.9461, 5.46287, 329.051), glm::vec3(107.48, 12.3011, 277.543), glm::vec3(40.4498, 4.29135, 235.217), glm::vec3(134.576, 20.7688, 432.295), glm::vec3(106.079, 14.0859, 519.444), glm::vec3(156.429, 21.5796, 519.444), glm::vec3(74.3315, 6.08642, 691.226), glm::vec3(110.515, 8.18049, 768.082), glm::vec3(68.5657, 4.08783, 826.784), glm::vec3(45.7547, 3.7867, 879.407), glm::vec3(170.735, 4.56332, 895), glm::vec3(179.3, 4.86985, 895), glm::vec3(258.398, 9.57684, 883.954), glm::vec3(258.398, 18.8671, 705.291), glm::vec3(258.398, 19.6358, 778.628), glm::vec3(345.181, 36.1556, 725.955), glm::vec3(245.424, 4.98389, 914.566), glm::vec3(220.88, 12.319, 734.062) };
	ObjectInWorld* allTree5[18];
	for(int i = 0; i < 18; i++)
	{
		allTree5[i] = new ObjectInWorld(&ourModelTree5, &allModelsTree5[i], 'N', false, 'a');
	}


	Model ourModelTree6(glm::vec3(0.0f, 4.5f, 0.0f), glm::vec3(2.f, 4.5f, 2.0f), 'N');
	glm::vec3 allModelsTree6[59] = { glm::vec3(109.711, 7.736, 791.937), glm::vec3(95.0145, 6.4164, 822.899), glm::vec3(117.03, 5.74055, 866.65), glm::vec3(124.438, 5.9906, 866.65), glm::vec3(126.59, 5.41146, 876.84), glm::vec3(153.156, 5.2686, 887.854), glm::vec3(136.537, 3.49832, 920.804), glm::vec3(193.057, 3.34361, 920.804), glm::vec3(163.066, 3.6516, 897.954), glm::vec3(236.833, 6.4381, 890.773), glm::vec3(195.244, 4.7437, 897.14), glm::vec3(202.306, 8.8995, 765.523), glm::vec3(194.614, 8.75435, 753.56), glm::vec3(207.896, 9.583, 785.925), glm::vec3(198.604, 9.04176, 772.423), glm::vec3(255.911, 15.258, 817.161), glm::vec3(268.485, 18.5757, 814.022), glm::vec3(241.553, 12.3222, 813.088), glm::vec3(241.553, 14.7862, 766.86), glm::vec3(167.823, 3.11934, 923.67), glm::vec3(225.559, 2.89314, 923.67), glm::vec3(275.902, 11.8174, 877.194), glm::vec3(264.49, 3.90606, 922.677), glm::vec3(277.634, 6.41441, 906.608), glm::vec3(331.443, 12.7029, 871.144), glm::vec3(205.807, 13.5527, 666.504), glm::vec3(215.72, 13.4025, 692.922), glm::vec3(247.384, 15.2878, 722.176), glm::vec3(219.053, 15.3242, 657.252), glm::vec3(242.436, 15.6477, 689.883), glm::vec3(157.899, 15.4705, 579.633), glm::vec3(137.913, 12.8306, 579.633), glm::vec3(115.263, 9.4185, 616.961), glm::vec3(82.3404, 7.9956, 566.485), glm::vec3(71.9907, 6.2882, 631.479), glm::vec3(113.768, 8.3616, 670.378), glm::vec3(131.848, 8.9385, 670.378), glm::vec3(110.611, 8.486, 749.811), glm::vec3(81.5625, 6.6464, 749.811), glm::vec3(81.5625, 5.4786, 793.209), glm::vec3(118.283, 6.85, 843.601), glm::vec3(103.413, 3.4910, 909.188), glm::vec3(124.705, 3.3614, 909.188), glm::vec3(80.8246, 4.308, 855.946), glm::vec3(64.5729, 2.8856, 855.946), glm::vec3(59.3116, 2.7460, 888.525), glm::vec3(101.015, 3.3161, 879.384), glm::vec3(153.662, 3.038, 919.953), glm::vec3(97.7035, 8.8641, 699.548), glm::vec3(134.446, 9.6289, 625.295), glm::vec3(148.061, 11.6617, 600.204), glm::vec3(131.612, 15.7015, 549.89), glm::vec3(105.359, 8.3736, 585.873), glm::vec3(62.33, 6.22796, 535.871), glm::vec3(62.33, 3.6128, 811.439), glm::vec3(98.8859, 7.9415, 754.26), glm::vec3(84.1168, 6.6579, 728.052), glm::vec3(253.019, 30.3453, 599.188), glm::vec3(276.429, 22.6476, 792.413) };
	ObjectInWorld* allTree6[59];
	for(int i = 0; i < 59; i++)
	{
		allTree6[i] = new ObjectInWorld(&ourModelTree6, &allModelsTree6[i], 'N', false, 'a');
	}


	Model ourModelHouse1(glm::vec3(0.0f, 43.0f, 0.0f), glm::vec3(36.f, 43.f, 64.0f), 'N');
	glm::vec3 positionHouse1 = glm::vec3(888.629, 3.169, 94.5);
	ObjectInWorld *house1 = new ObjectInWorld(&ourModelHouse1, &positionHouse1, 'N', false, 'a');


	Model ourModelRock1(glm::vec3(0.0f, 11.056f, -1.02f), glm::vec3(13.45f, 13.f, 22.95f), 'N');
	glm::vec3 positionRock1 = glm::vec3(825.678f, 4.567f, 628.302f);
	ObjectInWorld *rock1 = new ObjectInWorld(&ourModelRock1, &positionRock1, 'N', false, 'a');
	map1.tileMap->FindVector(positionRock1)->push_back(rock1);





	// start chrono
	auto start = std::chrono::steady_clock::now();

	//------------FIN PREPARATION BEFORE LOOP---------------