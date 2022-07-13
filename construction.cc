#include "construction.hh"
MyDetectorConstruction::MyDetectorConstruction()
{
	nCols=10;
	nRows=10;
	isCherenkov=false;
	isScintillator=true;
	fMessenger=new G4GenericMessenger(this,"/detector/","Detector Construction");
	fMessenger->DeclareProperty("nCols",nCols,"Number of columns");
	fMessenger->DeclareProperty("nRows",nRows,"Number of rows");
	fMessenger->DeclareProperty("isCherenkov",isCherenkov,"toggle Cherenkov setup");
	fMessenger->DeclareProperty("isScitillator",isScintillator,"toggle Scitillator setup");

	DefineMaterials();
	xWorld=0.5*m;
	yWorld=0.5*m;
	zWorld=0.5*m;
}
MyDetectorConstruction::~MyDetectorConstruction()
{}

void MyDetectorConstruction::DefineMaterials(){
	G4NistManager *nist=G4NistManager::Instance();
	
	SiO2=new G4Material("SiO2",2.201*g/cm3,2);
	SiO2->AddElement(nist->FindOrBuildElement("Si"),1);
	SiO2->AddElement(nist->FindOrBuildElement("O"),2);

	H2O=new G4Material("H2O",1.00*g/cm3,2);
	H2O->AddElement(nist->FindOrBuildElement("H"),2);
	H2O->AddElement(nist->FindOrBuildElement("O"),1);
	
	C=nist->FindOrBuildElement("C");

	Aerogel=new G4Material("Aerogel",0.200*g/cm3,3);
	Aerogel->AddMaterial(SiO2,62.5*perCent);
	Aerogel->AddMaterial(H2O,37.4*perCent);
	Aerogel->AddElement(C,0.1*perCent);
	worldMat=nist->FindOrBuildMaterial("G4_AIR");

	G4double energy[2]={1.239841939*eV/0.9,1.239841939*eV/0.2};
	G4double rindexAerogel[2]={1.1,1.1};
	G4double rindexWorld[2]={1.0,1.0};

	G4MaterialPropertiesTable *mptAerogel=new G4MaterialPropertiesTable();
	mptAerogel->AddProperty("RINDEX",energy,rindexAerogel,2);

	G4MaterialPropertiesTable *mptWorld=new G4MaterialPropertiesTable();
	mptWorld->AddProperty("RINDEX",energy,rindexWorld,2);
	Aerogel->SetMaterialPropertiesTable(mptAerogel);
	worldMat->SetMaterialPropertiesTable(mptWorld);

	Na=nist->FindOrBuildElement("Na");
	I=nist->FindOrBuildElement("I");
	NaI=new G4Material("NaI",3.67*g/cm3,2);
	NaI->AddElement(Na,1);
	NaI->AddElement(I,1);

}

void MyDetectorConstruction::ConstructCherenkov(){
	solideRadiator=new G4Box("solideRadiator",0.4*m,0.4*m,0.01*m);
	logicRadiator=new G4LogicalVolume(solideRadiator,
		Aerogel,"logicRadiator");
	fScoringVolume=logicRadiator;
	physRadiator=new G4PVPlacement(0,G4ThreeVector(0.,0.,0.25*m),logicRadiator,
		"physRadiator",logicWorld,false, 0,true);
	solidDetector= new G4Box("solidDetector",xWorld/nRows,yWorld/nCols,0.01*m);
	logicDetector= new G4LogicalVolume(solidDetector, worldMat,"logicDetector");
	
	for (G4int i=0;i<nRows;i++){
		for(G4int j=0;j<nCols; j++){
			physDetector=new G4PVPlacement(0,
				G4ThreeVector(-0.5*m+(i+0.5)*m/nRows,-0.5*m+(j+0.5)*m/nCols,0.49*m),
				logicDetector,"physDetector",logicWorld,false,j+i*nCols,true);
		}
	}

}

void MyDetectorConstruction::ConstructScintillator(){
	solidScintillator=new G4Tubs("solidScintillator",10*cm,20*cm,30*cm,
		0*deg,360*deg);
	logicScintillator=new G4LogicalVolume(solidScintillator,NaI,"logicScintillator");
	fScoringVolume=logicScintillator;
	physScintillator=new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),logicScintillator,
		"physScintillator",logicWorld,false,0,true);
}
G4VPhysicalVolume * MyDetectorConstruction::Construct()
{

	solidWorld=new G4Box("solidWorld",xWorld,yWorld,zWorld);
	logicWorld=new G4LogicalVolume(solidWorld,worldMat,
	 "logicWorld");
	physWorld=new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),
		logicWorld,"physWorld",0,false,0,true);
	if(isCherenkov)
		ConstructCherenkov();
	if(isScintillator)
		ConstructScintillator();
	
	return physWorld;

}

void MyDetectorConstruction::ConstructSDandField()
{
	MySensitiveDetector *sensDet=new MySensitiveDetector("SensitiveDetector");
	if(isCherenkov)
		logicDetector->SetSensitiveDetector(sensDet);
}



















