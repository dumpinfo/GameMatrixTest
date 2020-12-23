//=============================================================
//
// C4 Engine version 4.5
// Copyright 1999-2015, by Terathon Software LLC
//
// This file is part of the C4 Engine and is provided under the
// terms of the license agreement entered by the registed user.
//
// Unauthorized redistribution of source code is strictly
// prohibited. Violators will be prosecuted.
//
//=============================================================


#ifndef C4TerrainBuilders_h
#define C4TerrainBuilders_h


#include "C4Configuration.h"
#include "C4Terrain.h"


namespace C4
{
	typedef Type	TerrainBuilderType;


	enum
	{
		kTerrainBuilderFlatPlane			= 'PLAN',
		kTerrainBuilderTargaHeightField		= 'TGAH',
		kTerrainBuilderRawHeightField		= 'RAWH',
		kTerrainBuilderRawVoxelMap			= 'RAWV',
		kTerrainBuilderCount				= 4
	};


	struct TargaHeader;
	class TerrainBuilder;
	class Editor;


	class RawResource : public Resource<RawResource>
	{
		friend class Resource<RawResource>;

		private:

			static ResourceDescriptor	descriptor;

			~RawResource();

		public:

			RawResource(const char *name, ResourceCatalog *catalog);

			const unsigned_int8 *GetImageData(void) const
			{
				return (static_cast<const unsigned_int8 *>(GetData()));
			}

			const float *GetVoxelMapData(void) const
			{
				return (static_cast<const float *>(GetData()));
			}
	};


	class TerrainBuilder : public Configurable
	{
		private:

			TerrainBuilderType		terrainBuilderType;

		protected:

			TerrainBuilder(TerrainBuilderType type);

		public:

			virtual ~TerrainBuilder();

			TerrainBuilderType GetTerrainBuilderType(void) const
			{
				return (terrainBuilderType);
			}

			virtual bool BuildTerrain(TerrainBlock *block) = 0;
	};


	class FlatPlaneTerrainBuilder : public TerrainBuilder
	{
		private:

			Integer2D	planeSize;
			int32		emptyHeight;
			int32		solidDepth;

		public:

			FlatPlaneTerrainBuilder();
			~FlatPlaneTerrainBuilder();

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool BuildTerrain(TerrainBlock *block);
	};


	class TargaHeightFieldTerrainBuilder : public TerrainBuilder
	{
		private: 

			ResourceName	imageName;
			float			heightScale; 
			int32			emptyHeight;
			int32			solidDepth; 

			static unsigned_int8 *GenerateHeightField(const Color4C *image, const Integer2D& size, int32 fieldWidth, int32 fieldHeight);
 
		public:
 
			TargaHeightFieldTerrainBuilder(); 
			~TargaHeightFieldTerrainBuilder();

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override; 
			void SetSetting(const Setting *setting) override;

			bool BuildTerrain(TerrainBlock *block);
	};


	class RawHeightFieldTerrainBuilder : public TerrainBuilder
	{
		private:

			ResourceName	imageName;
			int32			imageWidth;
			int32			imageHeight;
			int32			channelCount;
			int32			channelDepth;

			float			heightScale;
			int32			emptyHeight;
			int32			solidDepth;

			unsigned_int8 *GenerateHeightField8(const unsigned_int8 *data, int32 fieldWidth, int32 fieldHeight) const;
			unsigned_int16 *GenerateHeightField16(const unsigned_int8 *data, int32 fieldWidth, int32 fieldHeight) const;

		public:

			RawHeightFieldTerrainBuilder();
			~RawHeightFieldTerrainBuilder();

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool BuildTerrain(TerrainBlock *block);
	};


	class RawVoxelMapTerrainBuilder : public TerrainBuilder
	{
		private:

			ResourceName	voxelMapName;
			int32			voxelMapWidth;
			int32			voxelMapHeight;
			int32			voxelMapDepth;

			int32			upDirectionIndex;

		public:

			RawVoxelMapTerrainBuilder();
			~RawVoxelMapTerrainBuilder();

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool BuildTerrain(TerrainBlock *block);
	};


	class BuildTerrainWindow : public Window
	{
		private:

			Editor					*worldEditor;
			TerrainBlock			*blockNode;

			TerrainBuilder			*currentBuilder;
			TerrainBuilder			*terrainBuilder[kTerrainBuilderCount];

			PushButtonWidget		*buildButton;
			PushButtonWidget		*cancelButton;

			ListWidget				*builderList;
			ConfigurationWidget		*configurationWidget;

		public:

			BuildTerrainWindow(Editor *editor, TerrainBlock *block);
			~BuildTerrainWindow();

			void Preprocess(void) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class TerrainProgressWindow : public Window
	{
		private:

			Editor					*worldEditor;
			TerrainBlock			*blockNode;
			TerrainBuilder			*terrainBuilder;

			Job						terrainJob;
			volatile bool			buildSuccess;
			Array<Geometry *>		geometryArray;

			PushButtonWidget		*stopButton;
			ProgressWidget			*progressBar;

			static void JobBuildTerrain(Job *job, void *cookie);

		public:

			TerrainProgressWindow(Editor *editor, TerrainBlock *block, TerrainBuilder *builder);
			~TerrainProgressWindow();

			void Preprocess(void) override;
			void Move(void) override;

			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};
}


#endif

// ZYUQURM
