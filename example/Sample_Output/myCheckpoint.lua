function LoadTheCheckpoint()
checkpoint = checkpoint or {}
	checkpoint["ugargv"] = 	checkpoint["ugargv"] or {}
		checkpoint["ugargv"][1] = "/home/paul/ug4/bin/ugshell"
		checkpoint["ugargv"][2] = "-ex"
		checkpoint["ugargv"][3] = "/home/paul/Schreibtisch/LUAOutputReader/Biogas.lua"
		checkpoint["ugargv"][4] = "-p"
		checkpoint["ugargv"][5] = "outputSpecification_08102020_2125.lua"
	checkpoint["lastFilename"] = "myCheckpointSimulationEnd.ug4vec"
	checkpoint["lastId"] = "SimulationEnd"
	checkpoint["commandline"] = "/home/paul/ug4/bin/ugshell -ex /home/paul/Schreibtisch/LUAOutputReader/Biogas.lua -p outputSpecification_08102020_2125.lua "
	checkpoint["ugargc"] = 5
	checkpoint["stdData"] = 	checkpoint["stdData"] or {}
		checkpoint["stdData"]["numCores"] = 1
return checkpoint
end
