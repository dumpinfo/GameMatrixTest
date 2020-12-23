 

#ifndef C4ConfigData_h
#define C4ConfigData_h


//# \component	System Utilities
//# \prefix		System/


#include "C4Resources.h"
#include "C4OpenDDL.h"


namespace C4
{
	enum : StructureType
	{
		kStructureVariable				= 'VRBL',
		kStructureVisit					= 'VIST',
		kStructureHistory				= 'HIST',
		kStructureDevice				= 'DEVC',
		kStructureControl				= 'CTRL',
		kStructureAction				= 'ACTN'
	};


	enum : DataResult
	{
		kDataConfigVariableInvalid		= 'CVRI',
		kDataConfigVisitInvalid			= 'CVSI',
		kDataConfigResourceMissing		= 'CRSM'
	};


	class ConfigResource : public Resource<ConfigResource>
	{
		friend class Resource<ConfigResource>;

		private:

			static C4API ResourceDescriptor		descriptor;

			~ConfigResource();

		public:

			C4API ConfigResource(const char *name, ResourceCatalog *catalog);

			const char *GetText(void) const
			{
				return (static_cast<const char *>(GetData()));
			}
	};


	class VariableStructure : public Structure
	{
		private:

			String<>	variableName;

		public:

			VariableStructure();
			~VariableStructure();

			bool ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value) override;
			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;

			DataResult ProcessData(DataDescription *dataDescription) override;
	};


	class VisitStructure : public Structure
	{
		private:

			Type		visitType;

		public:

			VisitStructure();
			~VisitStructure();

			bool ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value) override;
			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;

			DataResult ProcessData(DataDescription *dataDescription) override;
	};


	class HistoryStructure : public Structure
	{
		public:

			HistoryStructure();
			~HistoryStructure();

			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;

			DataResult ProcessData(DataDescription *dataDescription) override;
	}; 


	class DeviceStructure : public Structure 
	{
		private: 

			String<>	deviceName;
			Type		deviceType; 

		public: 
 
			DeviceStructure();
			~DeviceStructure();

			bool ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value) override; 
			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;

			DataResult ProcessData(DataDescription *dataDescription) override;
	};


	class ControlStructure : public Structure
	{
		friend class DeviceStructure;

		private:

			String<>	controlName;

		public:

			ControlStructure();
			~ControlStructure();

			bool ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value) override;
			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
	};


	class ActionStructure : public Structure
	{
		public:

			ActionStructure();
			~ActionStructure();

			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
	};


	//# \class	ConfigDataDescription		The data description subclass for the engine's configuration files.
	//
	//# The $ConfigDataDescription$ class is the data description subclass for the engine's configuration files.
	//
	//# \def	class ConfigDataDescription : public DataDescription
	//
	//# \desc
	//# The $ConfigDataDescription$ class represents an engine configuration file. It constructs ODDL data structures
	//# that appear in the file so that persistent configuration data can be read when the engine starts up.
	//#
	//# The $ConfigDataDescription$ class also contains the functionality needed to write the engine configuration
	//# and input control configuration files. Static member functions that can generate these files are provided.
	//
	//# \base	DataDescription		The config data description defines a specific ODDL format.


	//# \function	ConfigDataDescription::WriteEngineConfig		Writes the engine configuration to a file.
	//
	//# \proto	static void WriteEngineConfig(const char *name = C4_ENGINE_CONFIG_FILE);
	//
	//# \param	name	The name of the file.
	//
	//# \desc
	//# The $WriteEngineConfig$ function writes the values of all system variables to a file whose name is specified
	//# by the $name$ parameter. The file is written to a subfolder of the roaming application support directory for
	//# the current user, as defined by the operating system. The subfolder's name is that of the running application,
	//# defined by the $APPLICATION_NAME$ identifier.
	//#
	//# Additional data saved by the core engine is also written to the configuration file. This includes the most
	//# recent directories visited by the user to select various types of resources as well as the command history
	//# for the console window.
	//
	//# \also	$@ConfigDataDescription::WriteInputConfig@$
	//# \also	$@Engine::GetVariable@$
	//# \also	$@Engine::AddVariable@$
	//# \also	$@Variable@$
	//
	//# \wiki	File_Locations		File Locations


	//# \function	ConfigDataDescription::WriteInputConfig		Writes the input control configuration to a file.
	//
	//# \proto	static void WriteInputConfig(const char *name = C4_INPUT_CONFIG_FILE);
	//
	//# \param	name	The name of the file.
	//
	//# \desc
	//# The $WriteInputConfig$ function writes the values of all currently bound input controls to a file whose name
	//# is specified by the $name$ parameter. The file is written to a subfolder of the roaming application support
	//# directory for the current user, as defined by the operating system. The subfolder's name is that of the running
	//# application, defined by the $APPLICATION_NAME$ identifier.
	//
	//# \also	$@ConfigDataDescription::WriteEngineConfig@$
	//
	//# \wiki	File_Locations		File Locations


	class ConfigDataDescription : public DataDescription
	{
		public:

			ConfigDataDescription();
			~ConfigDataDescription();

			Structure *CreateStructure(const String<>& identifier) const override;
			bool ValidateTopLevelStructure(const Structure *structure) const override;

			C4API static DataResult Execute(const char *name, ResourceCatalog *catalog = nullptr);

			C4API static void WriteEngineConfig(const char *name = C4_ENGINE_CONFIG_FILE);
			C4API static void WriteInputConfig(const char *name = C4_INPUT_CONFIG_FILE);
	};
}


#endif

// ZYUQURM
