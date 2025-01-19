//END LOOP, MANAGE TIME-----------------

	auto end = std::chrono::steady_clock::now();
	auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	if (diff < InfoGlobal::frameTimeInt)
	{
		Sleep(InfoGlobal::frameTimeInt - diff);
		InfoGlobal::coefFrame = 1;
	}

	else if(diff < InfoGlobal::frameTimeInt + 10) InfoGlobal::coefFrame = 1;

	else
	{
		InfoGlobal::coefFrame = static_cast<int>(floor(diff / InfoGlobal::frameTimeInt) + 1);
		Sleep(InfoGlobal::frameTimeInt * InfoGlobal::coefFrame  - diff);
	}

//END LOOP, MANAGE TIME----------------