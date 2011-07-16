


#include <vector>
//#include <string.h>
#include <iostream> // standard streams
#include <fstream> // file streams
#include <sstream> // string streams
#include <map>
#include "floorPlanning.h"

using namespace std;

double floorPlanning(FloorPlan &fp)
{
	fp.setInOrder();
	//fp.list_information();
	fp.show_modules();
	SuperModules superModules;
/*	for(int i=0; i < fp.modules.size();i++){
		if((i % 4) == 0) {
			SuperModule dummy_mod;
			if( i+4 > fp.modules.size()) 
				dummy_mod.setChild(fp.modules,i,fp.modules.size()%4,true);
			else
				dummy_mod.setChild(fp.modules,i,4,true);
			superModules.push_back(dummy_mod);
		}
		
	}*/

	for(int i=0; i < fp.modules.size();i++){
		SuperModule dummy_mod;
		dummy_mod.setChild(fp.modules,i,1,true);
		superModules.push_back(dummy_mod);
	} 
	
	/*for(int i=0; i < fp.modules.size();i++){
		SuperModule dummy_mod;
		dummy_mod.setChild(fp.modules,fp.modules.size()-i-1,1,true);
		superModules.push_back(dummy_mod);
	}*/

	for( i=0; i < superModules.size();i++){
		superModules[i].arrangeBlocks();
		//superModules[i].showModules();
	}

	recursiveBottomUp(superModules);

	//for( i=0; i < superModules.size();i++)/
	//	superModules[i].setChildCordinates();
	//}

	for( i=0; i < superModules.size();i++){
			//newSuperModules[i].setChildCordinates();
			//superModules[i].showXYZModules();
	}
	
	for( i=0; i < superModules.size();i++){
		if(!superModules[i].isRotation) {
			fp.modules_info[i].x = superModules[i].x;
			fp.modules_info[i].y = superModules[i].y;
			fp.modules_info[i].rx = superModules[i].x + superModules[i].width;
			fp.modules_info[i].ry = superModules[i].y + superModules[i].height;
			//cout << "x===" << superModules[i].x << endl;
			//cout << "y===" << superModules[i].y << endl;

		} else {
			fp.modules_info[i].x = superModules[i].x;
			fp.modules_info[i].y = superModules[i].y;
			fp.modules_info[i].rx = superModules[i].x + superModules[i].height;
			fp.modules_info[i].ry = superModules[i].y + superModules[i].width;
			//cout << "x===" << superModules[i].x << endl;
			//cout << "y===" << superModules[i].y << endl;
		}
	}
	fp.Width =	finalWidth;
	fp.Height=	finalHeight;
	fp.Area = fp.Width * fp.Height;

	double time=seconds(); 
	return time; 
}


void recursiveBottomUp(SuperModules &superModules) {
	SuperModules newSuperModules;
	for(int i=0; i < superModules.size();i++){
		if((i % 4) == 0) {
			SuperModule dummy_mod;
			if( i+4 > superModules.size()) 
				dummy_mod.setChildComposite(superModules,i,superModules.size()%4,false);
			else
				dummy_mod.setChildComposite(superModules,i,4,false);
			newSuperModules.push_back(dummy_mod);
		}
		
	}
	
	for( i=0; i < newSuperModules.size();i++){
		newSuperModules[i].arrangeBlocks();
		//newSuperModules[i].showXYModules();
	}
	setInOrder(newSuperModules);
	if (newSuperModules.size() == 1) {
	//	cout << " final area ---- >>>>>> "  << newSuperModules[0].area << endl;
		finalWidth =  newSuperModules[0].width;
		finalHeight = newSuperModules[0].height;
	//	cout << " final width ---- >>>>>> "  << finalWidth << endl;
	//	cout << " final width ---- >>>>>> "  << finalHeight << endl;
		
		newSuperModules[0].isRotation = false;
		newSuperModules[0].setCordinates(0,0);
		//newSuperModules[0].showXYModules();
		newSuperModules[0].setChildCordinates();
		return;
	}
	else { 
		recursiveBottomUp(newSuperModules);
		if( newSuperModules.size() != 1) {
			for( i=0; i < newSuperModules.size();i++){
				newSuperModules[i].setChildCordinates();
				//newSuperModules[i].showXYModules();
			}
		}
		return;
	}

}