
	// STB_IMAGE.H flip loaded texture's on the y-axis
	stbi_set_flip_vertically_on_load(true);

	//timer ordonanceur
	timeBeginPeriod(5);



	// init variables generals
	Player *allPlayers = static_cast<Player*>(calloc(static_cast<size_t>(InfoGlobal::nMaxClients), sizeof(Player)));
	for(int i=0; i<InfoGlobal::nMaxClients; i++){ allPlayers[i].connected = false; allPlayers[i].stateInfo = 'n'; allPlayers[i].timeWithoutSendState = 0;}
	uint16_t nPlayersActualize = 0; uint16_t listPlayersActualize[InfoGlobal::nMaxClients]; // nb de player a qui ont modifies leur etat sur cette frame




	//init variable network
	InfoGlobal::myUdp = new Udp(NPORT);
    DatagramClientReceived datagramsReceived; datagramsReceived.allDatagrams = static_cast<DatagramByClient*>(calloc(static_cast<size_t>(SIZE_TAMPON_RECEIVED), sizeof(DatagramByClient))); datagramsReceived.pullEmplacement = 0; datagramsReceived.pushEmplacement = 0;
	//start network thread
	std::thread t1(ManageAllDatagramsReceived, std::ref(datagramsReceived));



