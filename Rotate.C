/*
This script will take a given set of co-ordinates and rotate them to another
and displaying the outcome.

experiment: the experiment which you want to rotate the coordinates by: uboone, nova, minverva
USAGE: root -l 'Rotate.C("experiment name")'

OPTIONS: Added a bool in the uboone code section gsimple. Set to true/false if you need to 
		 investigate which coo-rdinate system you want to use. 

*/

// Functions
// ------------------------------------------------------------------------------------------
//  Returns a TVector using a theta phi rotation
TVector3 AnglesToAxis(const double theta, const double phi) {
	
	// Theta, Phi in rad
	double xyz[3];

	xyz[0] = TMath::Cos(phi) * TMath::Sin(theta);
	xyz[1] = TMath::Sin(phi) * TMath::Sin(theta);
	xyz[2] = TMath::Cos(theta);

	// Condition vector to eliminate most floating point errors
	for (int i=0; i<3; ++i) {
		
		const double eps = 1.0e-15;
		if (TMath::Abs(xyz[i])   < eps ) xyz[i] =  0;
		if (TMath::Abs(xyz[i]-1) < eps ) xyz[i] =  1;
		if (TMath::Abs(xyz[i]+1) < eps ) xyz[i] = -1;
	}
	
	return TVector3(xyz[0], xyz[1], xyz[2]);                    
}
// ------------------------------------------------------------------------------------------
// Returns the correct TRotation from input rotation matrix
void GetRotation(const std::vector<double> rotmatrix, TRotation &RotDet2Beam, TRotation &RotBeam2Det ){
	
	// Matrix defined with series of rotations
	if (rotmatrix.size() == 3) {
		
		std::cout << "\nUsing rotmatrix defined with size:\t" << rotmatrix.size() << std::endl;
		
		RotDet2Beam.RotateX(rotmatrix[0]);
		RotDet2Beam.RotateY(rotmatrix[1]);
		RotDet2Beam.RotateZ(rotmatrix[2]);
	}
	else {
		TVector3 newX, newY, newZ;

		// Matrix defined with new axis values
		if (rotmatrix.size() == 9) {

			std::cout << "\nUsing rotmatrix defined with size:\t" << rotmatrix.size() << std::endl;

			newX = TVector3(rotmatrix[0], rotmatrix[1], rotmatrix[2]);
			newY = TVector3(rotmatrix[3], rotmatrix[4], rotmatrix[5]);
			newZ = TVector3(rotmatrix[6], rotmatrix[7], rotmatrix[8]);
	  	}
		// Matrix defined with theta phi rotations
		else if (rotmatrix.size() == 6) {

			std::cout << "\nUsing rotmatrix defined with size:\t" << rotmatrix.size() << std::endl;
		
			
			newX = AnglesToAxis(rotmatrix[0], rotmatrix[1]);
			newY = AnglesToAxis(rotmatrix[2], rotmatrix[3]);
			newZ = AnglesToAxis(rotmatrix[4], rotmatrix[5]);
			RotDet2Beam.RotateAxes(newX,newY,newZ);
			RotBeam2Det = RotDet2Beam;
		}

	  RotDet2Beam.RotateAxes(newX, newY, newZ); // Return the TRotation and also inverts
	}

	RotBeam2Det = RotDet2Beam.Inverse();
	
	// Print the rotmatrix
	int w=10, p=20;
	std::cout << "\n Rotation matrix from beam to detector coordinates: " << std::setprecision(p) << std::endl;
	std::cout << " [ " 
			  << std::setw(w) << RotBeam2Det.XX() << " "
			  << std::setw(w) << RotBeam2Det.XY() << " "
			  << std::setw(w) << RotBeam2Det.XZ() << std::endl
			  << "   " 
			  << std::setw(w) << RotBeam2Det.YX() << " "
			  << std::setw(w) << RotBeam2Det.YY() << " "
			  << std::setw(w) << RotBeam2Det.YZ() << std::endl
			  << "   " 
			  << std::setw(w) << RotBeam2Det.ZX() << " "
			  << std::setw(w) << RotBeam2Det.ZY() << " "
			  << std::setw(w) << RotBeam2Det.ZZ() << " ] " << std::endl;
	std::cout << std::endl;
	
	std::cout << "\n Rotation matrix from detector to beam coordinates: " << std::setprecision(p) << std::endl;
	std::cout << " [ " 
			  << std::setw(w) << RotDet2Beam.XX() << " "
			  << std::setw(w) << RotDet2Beam.XY() << " "
			  << std::setw(w) << RotDet2Beam.XZ() << std::endl
			  << "   " 
			  << std::setw(w) << RotDet2Beam.YX() << " "
			  << std::setw(w) << RotDet2Beam.YY() << " "
			  << std::setw(w) << RotDet2Beam.YZ() << std::endl
			  << "   " 
			  << std::setw(w) << RotDet2Beam.ZX() << " "
			  << std::setw(w) << RotDet2Beam.ZY() << " "
			  << std::setw(w) << RotDet2Beam.ZZ() << " ] " << std::endl;
	std::cout << std::endl;

}
// ------------------------------------------------------------------------------------------
// Returns the correct translations from target to detector center
void GetCoordinates(const std::vector<double> detxyz, TVector3 &TransDetCoord, TVector3 &TransBeamCoord, const TRotation RotBeam2Det, const TRotation RotDet2Beam ){

	// Translation of beam to detector in detector coords given
	if (detxyz.size() == 3) {
		TransDetCoord = TVector3(detxyz[0], detxyz[1], detxyz[2]); 
		TVector3 DetOrigin = TVector3(0, 0, 0);
		TransBeamCoord = RotDet2Beam * (DetOrigin - TransDetCoord);
	}
	
	// Origin of the detector in detector coordinate system, translation of the target to detector in beam coordinates(in cm)
	else if (detxyz.size() == 6) {
		TVector3 DetOrigin = TVector3(detxyz[0], detxyz[1], detxyz[2]); // Detector 0,0,0
		TransBeamCoord = TVector3(detxyz[3], detxyz[4], detxyz[5]);
		TransDetCoord = DetOrigin - RotBeam2Det * TransBeamCoord;		// Translation of beam to detector in detector coords 
	}
	
	else std::cout << "detxyz needs 3 or 6 numbers to be properly defined" << std::endl;
	
	// Print the output
	std::cout << "\n Translation of target to detector in detector coords:\n  [ " << std::setprecision(10) 
			  << std::setw(16) << TransDetCoord.X() << " , "
			  << std::setw(16) << TransDetCoord.Y() << " , "
			  << std::setw(16) << TransDetCoord.Z() << " ] "
			  << std::endl;
	
	std::cout << "\n Translation of target to detector in beam coords:\n  [ " << std::setprecision(10) 
			  << std::setw(16) << TransBeamCoord.X() << " , "
			  << std::setw(16) << TransBeamCoord.Y() << " , "
			  << std::setw(16) << TransBeamCoord.Z() << " ] "
			  << std::endl;
}
// ------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------
// MAIN
void Rotate(std::string experiment) {

	// Variables
	TRotation RotBeam2Det, RotDet2Beam; 	// Rotations
	TVector3 TransDetCoord, TransBeamCoord;	// Translations
	std::vector<double> detxyz, rotmatrix;	// Inputs
	bool gsimple{false};

	/*
		• detxyxz: Give it an origin of detector in detector coordinates
		and a translation from target to detector in beam coordinates (vec of size 6)
									OR
		target to detector translation in detector (vec of size 3)

		• rotmatrix: Rotation matrix goes from beam to detector coordinates
	*/

	// uBooNE coordinates

	if (experiment == "uboone"){
		std::cout << "--------------------------" << std::endl;
		std::cout << "      \033[1;31m Using uBooNE!\033[0m" << std::endl;
		std::cout << "--------------------------" << std::endl;
		detxyz = {0, 0, 0, 5502, 7259, 67270}; //cm
		rotmatrix = {
					0.92103853804025682,		0.0227135048039241207,	0.38880857519374290,
					0.0000462540012621546684,	0.99829162468141475,	-0.0584279894529063024,
					-0.38947144863934974,		0.0538324139386641073,	0.91946400794392302};
		// rotmatrix = { -0.0583497, 0, 0}; // rotation matrix as specified in the GENIE Flux Driver
		
		/*
		// Flux window in detector coordinate system (in cm) -- cuttently used
		windowBase: [ 500, -500, -3500 ]
		window1:    [-500,  200, -3500 ]
		window2:    [ 500, -500,  2000 ]
		*/
		// ------------------------------------------------------------------------------------------
		// Coordinates and rotaion used in gsimple files
		if (gsimple){
			std::cout << "\033[1;31m Using GSimple Settings... \033[0m\n "<< std::endl; // colour red
			detxyz = {0, 0, 0, 5449.9, 7446.1, 67761.1}; //cm
			rotmatrix = {
						0.921229, 	0.0226873,	0.388359,
						0.00136256,	0.998104,	-0.0615396,
						-0.389019,	0.0572212,	0.919451}; 

			// Flux window in detector coordinates used -- looks equvalent
			// (x = 5, y = -5, z = -35)
			// (x = -5, y = 2, z = -35)
			// (x = 5, y = -5, z = 20)
		}
		// ------------------------------------------------------------------------------------------

	}
	// NOvA coordinates
	else if (experiment == "nova"){
		std::cout << "--------------------------" << std::endl;
		std::cout << "      Using NOvA!" << std::endl;
		std::cout << "--------------------------" << std::endl;
		// detxyz = { 226.9447, 6100.1882, -99113.1313}; //cm
		detxyz = {0, 0, 0, 1171.74545, -331.51325, 99293.47347}; // locations.txt
		rotmatrix = {
					9.9990e-01, -8.2300e-04, -1.4088e-02,
					3.0533e-06,  9.9831e-01, -5.8103e-02,
					1.4112e-02,  5.8097e-02,  9.9821e-01};
	}
	// MINERvA coordninates
	else if (experiment == "minerva"){
		std::cout << "--------------------------" << std::endl;
		std::cout << "     Using MINERvA!" << std::endl;
		std::cout << "--------------------------" << std::endl;
		// detxyz = {  24.86, 6035.0, -102274.0}; //cm
		detxyz = {  0, 0, 716.95,  -56.28, -53.29317, 103231.9}; // locations.txt
		rotmatrix = { -0.0582977560, 0, 0 }; // x,y,z rotations
	}
	// Unimplemented
	else {
		std::cout << "Not implemented the input detector or it is mispelled..." << std::endl;
		std::cout << "Choose one of: uboone, nova or minerva" << std::endl;
		gSystem->Exit(0); // quit
	}
	// Return the TRotation
	GetRotation(rotmatrix, RotDet2Beam, RotBeam2Det); 

	// Now get the correct coordinates
	GetCoordinates(detxyz, TransDetCoord, TransBeamCoord, RotBeam2Det, RotDet2Beam );

	gSystem->Exit(0); // quit
} // END 
