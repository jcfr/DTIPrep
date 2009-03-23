
#include <iostream>
#include <fstream>
#include <string>

#include <QApplication>
#include <QPushButton>
#include <QString>

#include "GMainWindow.h"
#include "main.h"

#include <stddef.h> /* size_t */
#include <string.h> /* strcmp */

#include "metaCommand.h"
#include "IntensityMotionCheck.h"

#include "Protocal.h"
#include "XmlStreamReader.h"

// #include <QStyleFactory>
using namespace std;
int main ( int argc, char ** argv )
{

	if(argc==1)
	{
		QApplication app (argc, argv);
		GMainWindow *MainWindow = new GMainWindow;
		MainWindow ->show();
		return app.exec();
	}
	else
	{
		MetaCommand command;

		// input
		command.SetOption(			"DWIFileName", "w", true,"DWI file name to convert dicom image series into or to be checked (nhdr)");
		command.SetOptionLongTag(	"DWIFileName","DWINrrdFile");
		command.AddOptionField(		"DWIFileName","DWIFileName",MetaCommand::STRING, true);

		// dicom to nrrd
		command.SetOption(			"Dicom2Nrrd", "d", false,"Directory containing dicom images to be converted");
		command.SetOptionLongTag(	"Dicom2Nrrd","DicomDirectory");
		command.AddOptionField(		"Dicom2Nrrd","InputDicomDir",MetaCommand::STRING,true);

		// xml based check
		command.SetOption(			"xmlSetting","p", false,"protocol xml file");
		command.SetOptionLongTag(	"xmlSetting","xmlProtocol");
		command.AddOptionField(		"xmlSetting","xmlFileName",MetaCommand::STRING, true);
	
		if( !command.Parse(argc,argv) )
			return EXIT_FAILURE;

		string DWIFileName	= command.GetValueAsString("DWIFileName","DWIFileName");
		string InputDicomDir	= command.GetValueAsString("Dicom2Nrrd","InputDicomDir");
		string xmlFileName	= command.GetValueAsString("xmlSetting","xmlFileName");

		cout<<"xmlFileName "<<xmlFileName<<endl;
// convert
		if(command.GetOptionWasSet("Dicom2Nrrd") && InputDicomDir.length()!=0 && DWIFileName.length()!=0  )
		{
			std::string str,str1,str2;
			str += string("DicomToNrrdConverter");			
			str += string("  ");
			str += InputDicomDir;
			str += string("  ");
			str += DWIFileName;
			
			cout<<"DicomToNrrd "<<InputDicomDir<<" "<<DWIFileName<<endl;
			system(const_cast<char *>(str.c_str())); 
		}

// check
		CIntensityMotionCheck IntensityMotionCheck( DWIFileName );
		// xml
		if(command.GetOptionWasSet("xmlSetting") )
		{			
			Protocal protocal;			
			QCResult qcResult;

			QString str( xmlFileName.c_str() );
			XmlStreamReader XmlReader(NULL);
			XmlReader.setProtocal( &protocal);
			XmlReader.readFile(str, XmlStreamReader::ProtocalWise);

			IntensityMotionCheck.SetProtocal( &protocal);
			IntensityMotionCheck.SetQCResult( &qcResult);
			IntensityMotionCheck.GetImagesInformation();
			IntensityMotionCheck.CheckByProtocal();
		}
		return 0;
	}
}