#include "custom_method.h"
#include "../core/global.h"

namespace ofec {

	void registerParamAbbr() {
		// ******* user-defined parameters ***
		g_param_abbr.insert({ "OA","objective accuracy" });
		g_param_abbr.insert({ "UVP","useVariablePartition" });
		g_param_abbr.insert({ "F","scaling factor" });
		g_param_abbr.insert({ "CR","crossover rate" });
		g_param_abbr.insert({ "NPa","numPartition" });
		g_param_abbr.insert({ "CS","changeSeverity" });//add by tanqingshan
		g_param_abbr.insert({ "DG","durationGeneration" });
		g_param_abbr.insert({ "DR","detectRatio" });
		g_param_abbr.insert({ "HMR","hyperMutaRate" });
		g_param_abbr.insert({ "NPS","numPS" });
		g_param_abbr.insert({ "RPS","radiusPS" });
		g_param_abbr.insert({ "RR","replaceRatio" });
		g_param_abbr.insert({ "TI","testItems" });//add by tanqingshan
		g_param_abbr.insert({ "W","weight" });
		g_param_abbr.insert({ "C1","accelerator1" });
		g_param_abbr.insert({ "C2","accelerator2" });
		g_param_abbr.insert({ "NAS","number of atomspaces" });
		g_param_abbr.insert({ "NSSP","number of subspaces" });
		g_param_abbr.insert({ "MSS","max subpop size" });// by pengmai
		g_param_abbr.insert({ "DD","diversity degree" });// by pengmai
		// ******* default parameters ********
		g_param_abbr.insert({ "AID","algId" });
		g_param_abbr.insert({ "al","alpha" });
		g_param_abbr.insert({ "AN","algorithm name" });
		g_param_abbr.insert({ "be","beta" });
		g_param_abbr.insert({ "ca","case" });
		g_param_abbr.insert({ "CDBGFID","comDBGFunID" });
		g_param_abbr.insert({ "CF","changeFre" });
		g_param_abbr.insert({ "CoF","convFactor" });
		g_param_abbr.insert({ "CPR","changePeakRatio" });
		g_param_abbr.insert({ "CT","changeType" });
		g_param_abbr.insert({ "CT2","changeType2" });
		g_param_abbr.insert({ "CTH","convThreshold" });
		g_param_abbr.insert({ "DD1","dataDirectory1" });
		g_param_abbr.insert({ "DF1","dataFile1" });
		g_param_abbr.insert({ "DF2","dataFile2" });
		g_param_abbr.insert({ "DF3","dataFile3" });
		g_param_abbr.insert({ "DM","divisionMode" });
		g_param_abbr.insert({ "ECF","evalCountFlag" });
		g_param_abbr.insert({ "ep","epsilon" });
		g_param_abbr.insert({ "et","eta" });
		g_param_abbr.insert({ "ER","exlRadius" });
		g_param_abbr.insert({ "FA","flagAsymmetry" });
		g_param_abbr.insert({ "FI","flagIrregular" });
		g_param_abbr.insert({ "FN","flagNoise" });
		g_param_abbr.insert({ "FNDC","flagNumDimChange" });
		g_param_abbr.insert({ "FNPC","flagNumPeakChange" });
		g_param_abbr.insert({ "FR","flagRotation" });
		g_param_abbr.insert({ "FTL","flagTimeLinkage" });
		g_param_abbr.insert({ "ga","gamma" });
		g_param_abbr.insert({ "gOptFlag","gOptFlag" });
		g_param_abbr.insert({ "GS","glstrture" });
		g_param_abbr.insert({ "HCM","heightConfigMode" });
		g_param_abbr.insert({ "HR","hibernatingRadius" });
		g_param_abbr.insert({ "IT1","interTest1" });
		g_param_abbr.insert({ "IT2","interTest2" });
		g_param_abbr.insert({ "IT3","interTest3" });
		g_param_abbr.insert({ "IT4","interTest4" });
		g_param_abbr.insert({ "JH","jumpHeight" });
		g_param_abbr.insert({ "KF","kFactor" });
		g_param_abbr.insert({ "LF","lFactor" });
		g_param_abbr.insert({ "ME","maximum evaluations" });
		g_param_abbr.insert({ "MI","maxIter" });
		g_param_abbr.insert({ "MNPS","minNumPopSize" });
		g_param_abbr.insert({ "MP","mutProbability" });
		g_param_abbr.insert({ "MSDE","mutation strategy" });
		g_param_abbr.insert({ "MSI","maxSucIter" });
		g_param_abbr.insert({ "NB","numBox" });
		g_param_abbr.insert({ "NC","numChange" });
		g_param_abbr.insert({ "NCus","numCus" });
		g_param_abbr.insert({ "ND","number of variables" });
		g_param_abbr.insert({ "NF","noiseFlag" });
		g_param_abbr.insert({ "NGO","numGOpt" });
		g_param_abbr.insert({ "NNCus","numNewCus" });
		g_param_abbr.insert({ "NO","number of objectives" });
		g_param_abbr.insert({ "NP","numPeak" });
		g_param_abbr.insert({ "NPR","numParetoRegion" });
		g_param_abbr.insert({ "NR","number of runs" });
		g_param_abbr.insert({ "NS","noiseSeverity" });
		g_param_abbr.insert({ "NSP","numSubPop" });
		g_param_abbr.insert({ "NT","numTask" });
		g_param_abbr.insert({ "OLD","overlapDgre" });
		g_param_abbr.insert({ "PC","peakCenter" });
		g_param_abbr.insert({ "PF","predicFlag" });
		g_param_abbr.insert({ "PID","proId" });
		g_param_abbr.insert({ "PIM","populationInitialMethod" });
		g_param_abbr.insert({ "PkS","peakShape" });
		g_param_abbr.insert({ "PN","problem name" });
		g_param_abbr.insert({ "PNCM","peakNumChangeMode" });
		g_param_abbr.insert({ "POS","peakOffset" });
		g_param_abbr.insert({ "PPB","peaksPerBox" });
		g_param_abbr.insert({ "PS","population size" });
		g_param_abbr.insert({ "PT","proTag" });
		g_param_abbr.insert({ "ra","radius" });
		g_param_abbr.insert({ "RBP","resource4BestPop" });
		g_param_abbr.insert({ "RID","runId" });
		g_param_abbr.insert({ "SF","sample frequency" });
		g_param_abbr.insert({ "SI","stepIndi" });
		g_param_abbr.insert({ "SL","shiftLength" });
		g_param_abbr.insert({ "SPS","subpopulation size" });
		g_param_abbr.insert({ "SVM","solutionValidationMode" });
		g_param_abbr.insert({ "TLS","timelinkageSeverity" });
		g_param_abbr.insert({ "TT","trainingTime" });
		g_param_abbr.insert({ "TW","timeWindow" });
		g_param_abbr.insert({ "USPL","updateSchemeProbabilityLearning" });
		g_param_abbr.insert({ "VR","validRadius" });
		g_param_abbr.insert({ "VR","variableRelation" });
		g_param_abbr.insert({ "XP","xoverProbability" });
		g_param_abbr.insert({ "DFR","dataFileResult" });
		g_param_abbr.insert({ "MRT","maxRunTime" });
	}

	void customizeFileName() {
		g_param_omit.insert("algId");
		g_param_omit.insert("dataDirectory1");
		g_param_omit.insert("evalCountFlag");
		g_param_omit.insert("flagNoise");
		g_param_omit.insert("flagNumPeakChange");
		g_param_omit.insert("flagTimeLinkage");
		g_param_omit.insert("gOptFlag");
		g_param_omit.insert("hibernatingRadius");
		g_param_omit.insert("maximum evaluations");
		g_param_omit.insert("minNumPopSize");
		g_param_omit.insert("numGOpt");
		g_param_omit.insert("number of runs");
		g_param_omit.insert("numTask");
		g_param_omit.insert("peakNumChangeMode");
		g_param_omit.insert("proId");
		g_param_omit.insert("sample frequency");
		g_param_omit.insert("solutionValidationMode");
	}
}
