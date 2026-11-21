//#pragma optimize("", off)

//----------------------------------------------------
//--- GAMEGURU - M-Entity
//----------------------------------------------------

#include "stdafx.h"
#include "gameguru.h"

// OPTICK Performance
#ifdef OPTICK_ENABLE
#include "optick.h"
#endif

#include "..\Imgui\imgui.h"
#include "..\Imgui\imgui_impl_win32.h"
#include "..\Imgui\imgui_gg_dx11.h"

#include ".\\..\..\\Guru-WickedMAX\\GPUParticles.h"
using namespace GPUParticles;
#include "GGTerrain\GGTerrain.h"
using namespace GGTerrain;

#include "..\..\..\..\Guru-WickedMAX\wickedcalls.h"
#include "WickedEngine.h"
using namespace std;
using namespace wiGraphics;
using namespace wiScene;
using namespace wiECS;

#define MAXREADYDECALS 5
#define MAXUNIQUEDECALS 100
uint32_t ready_decals[MAXUNIQUEDECALS][MAXREADYDECALS] = { 0 };
uint32_t decal_count[MAXUNIQUEDECALS] = { 0 };

// Globals for blacklist string array
LPSTR* g_pBlackList = NULL;
int g_iBlackListMax = 0;
bool g_bBlackListRemovedSomeEntities = false;

int g_iWickedEntityId = -1;
int g_iWickedElementId = 0;
int g_iWickedMeshNumber = 0;
bool g_bUseEditorGrideleprof = false;
int g_iAbortedAsEntityIsGroupFileMode = 0;
int g_iAbortedAsEntityIsGroupFileModeStubOnly = 0;
int g_iAbortedAsEntityIsGroupCreate = 0;
cstr g_sTempGroupForThumbnail = "";

float g_fFlattenMargin = 100.0f;

//. externs
extern std::vector<int> g_ObjectHighlightList;
#define MAXGROUPSLISTS 100 // duplicated in GridEdit.cpp (would replace this when the group list is dynamic)
extern std::vector<sRubberBandType> vEntityGroupList[MAXGROUPSLISTS];
extern cstr sEntityGroupListName[MAXGROUPSLISTS];
extern int GetGroupIndexFromName(cstr sLookFor);

// 
//  ENTITY COMMON CODE (not game specific)
// 

void entity_addtoselection_core ( void )
{
	//  ensure ENT$ does not contain duplicate \ symbols
	t.tnewent_s="";
	for ( t.n = 1 ; t.n<=  Len(t.ent_s.Get()); t.n++ )
	{
		t.tnewent_s=t.tnewent_s+Mid(t.ent_s.Get(),t.n);
		if ( cstr(Mid(t.ent_s.Get(),t.n)) == "\\" && cstr(Mid(t.ent_s.Get(),t.n+1)) == "\\" ) 
		{
			++t.n;
		}
	}
	t.ent_s=t.tnewent_s;

	// Load entity from file
	t.entdir_s="entitybank\\";
	t.ent_s=Right(t.ent_s.Get(),1+(Len(t.ent_s.Get())-(Len(g.rootdir_s.Get())+Len(t.entdir_s.Get()))));

	// Check if filename valid
	t.entnewloaded=0 ; t.entid=0;
	if ( cstr(Right(t.ent_s.Get(),4)) == ".fpe" ) 
	{
		// Check entity exists in bank
		t.tokay=1;
		if ( g.entidmaster>0 ) 
		{
			for ( t.entid = 1 ; t.entid<=  g.entidmaster; t.entid++ )
			{
				if ( t.entitybank_s[t.entid] == t.ent_s ) {  t.tokay = 0  ; t.tfoundid = t.entid ; break; }
			}
		}
		if ( t.tokay == 1 ) 
		{
			// Find Free entity Index
			t.freeentid=-1;
			if ( g.entidmaster>0 ) 
			{
				for ( t.entid = 1 ; t.entid <= g.entidmaster; t.entid++ )
				{
					if ( t.entityprofileheader[t.entid].desc_s == "" ) {  t.freeentid = t.entid  ; break; }
				}
			}

			//  New entity or Free One
			if ( t.freeentid == -1 ) 
			{
				++g.entidmaster ; entity_validatearraysize ( );
				t.entid=g.entidmaster;
				t.entnewloaded=1;
			}
			else
			{
				t.entid=t.freeentid;
			}

			// Load entity
			t.entitybank_s[t.entid]=t.ent_s;
			t.entpath_s=getpath(t.ent_s.Get());
			entity_load ( );
		}
		else
		{
			// already got, assign ID from existing
			t.entid=t.tfoundid;
		}
	}
}

void entity_addtoselection ( void )
{
	//  Load entity from file requester
	SetDir (  g.currententitydir_s.Get() );
	t.ent_s=browseropen_s(9);
	g.currententitydir_s=GetDir();
	SetDir (  g.rootdir_s.Get() );
	entity_addtoselection_core ( );
}

bool entity_copytoremoteifnotthere ( LPSTR pPathToFile )
{
	//PE: Bug fix , never copy in standalone.
	if (t.game.gameisexe == 1) return false;

	// this only works if destination folder already exists, use "mapfile_ensurethisfolderexistsinremoteproject" if want to copy whole folders!
	// if using remote project, first duplicate the entity file to local project
	bool bWeCopiedTheFileOver = false;
	extern StoryboardStruct Storyboard;
	char pPreferredProjectEntityFolder[MAX_PATH];
	strcpy(pPreferredProjectEntityFolder, Storyboard.customprojectfolder);
	strcat(pPreferredProjectEntityFolder, Storyboard.gamename);
	if (strlen(Storyboard.customprojectfolder) > 0)
	{
		// yes, a remote project
		char fullRealPath[MAX_PATH];
		strcpy(fullRealPath, pPathToFile);
		GG_GetRealPath(fullRealPath, 0);
		if (strnicmp (fullRealPath, pPreferredProjectEntityFolder, strlen(pPreferredProjectEntityFolder)) != NULL)
		{
			// file not in local project, copy it over
			strcpy(pPreferredProjectEntityFolder, pPathToFile);
			int ret = GG_GetRealPath(pPreferredProjectEntityFolder, 1);
			if( stricmp(pPreferredProjectEntityFolder, fullRealPath) != NULL )
				CopyFileA(fullRealPath, pPreferredProjectEntityFolder, TRUE);
			else if (ret == 0)
			{
				//PE: Remote project not found, use relative path.
				char Relative[MAX_PATH];
				const char* find = pestrcasestr(fullRealPath, "\\files\\");
				if (find)
				{
					strcpy(Relative, find + 7);
					int ret = GG_GetRealPath(Relative, 1);
					if (stricmp(Relative, fullRealPath) != NULL)
						CopyFileA(fullRealPath, Relative, TRUE);
				}
			}

			bWeCopiedTheFileOver = true;
		}
	}
	return bWeCopiedTheFileOver;
}

void entity_adduniqueentity ( bool bAllowDuplicates )
{
	// Ensure 'entitybank\' is not part of entity filename
	t.entdir_s="entitybank\\";
	if (  cstr(Lower(Left(t.addentityfile_s.Get(),11))) == "entitybank\\" ) 
	{
		t.addentityfile_s=Right(t.addentityfile_s.Get(),Len(t.addentityfile_s.Get())-11);
	}
	if (  cstr(Lower(Left(t.addentityfile_s.Get(),8))) == "ebebank\\" ) 
	{
		t.entdir_s = "";
	}

	//  Check if entity already loaded in
	t.talreadyloaded=0;
	if ( bAllowDuplicates == false )
	{
		for ( t.t = 1 ; t.t<=  g.entidmaster; t.t++ )
		{
			if (  t.entitybank_s[t.t] == t.addentityfile_s ) {  t.talreadyloaded = 1  ; t.entid = t.t; }
		}
	}
	if (t.talreadyloaded == 0)
	{
		//  Allocate one more entity item in array
		if (g.entidmaster > g.entitybankmax - 4)
		{
			Dim (t.tempentitybank_s, g.entitybankmax);
			for (t.t = 0; t.t <= g.entitybankmax; t.t++) t.tempentitybank_s[t.t] = t.entitybank_s[t.t];
			++g.entitybankmax;
			UnDim (t.entitybank_s);
			Dim (t.entitybank_s, g.entitybankmax);
			for (t.t = 0; t.t <= g.entitybankmax - 1; t.t++) t.entitybank_s[t.t] = t.tempentitybank_s[t.t];
		}

		//  Add entity to bank
		++g.entidmaster; entity_validatearraysize ();
		t.entitybank_s[g.entidmaster] = t.addentityfile_s;

		// trigger the creation of a 'group' entity if detected
		if (g_iAbortedAsEntityIsGroupFileMode != 3)
			g_iAbortedAsEntityIsGroupFileMode = 1;

		// if using remote project, first duplicate the entity file to local project
		bool bAlsoCopyOverAllRelatedEntityFiles = false;
		char pThisEntityFile[MAX_PATH];
		strcpy(pThisEntityFile, "entitybank\\");
		strcat(pThisEntityFile, t.addentityfile_s.Get());
		if (entity_copytoremoteifnotthere(pThisEntityFile) == true)
		{
			// and if successfully copied over, copy over all related files
			bAlsoCopyOverAllRelatedEntityFiles = true;
		}

		//  Load extra entity
		t.entid = g.entidmaster;
		t.ent_s = t.entitybank_s[t.entid];
		t.entpath_s = getpath(t.ent_s.Get());
		extern uint32_t SetMasterObject;
		SetMasterObject = g.entitybankoffset + t.entid;
		entity_load ();
		SetMasterObject = 0;

		// copy over all related files if using a remote project
		if (bAlsoCopyOverAllRelatedEntityFiles == true)
		{
			extern bool g_bMakingAStandaloneUsingFileCollectionArray;
			if (g_bMakingAStandaloneUsingFileCollectionArray == false)
			{
				// clear file collection
				g.filecollectionmax = 0;
				Undim (t.filecollection_s);
				Dim (t.filecollection_s, 500);

				// collect all the associated files into filecollection
				extern void mapfile_addallentityrelatedfiles(int, entityeleproftype*);
				entity_fillgrideleproffromprofile();
				mapfile_addallentityrelatedfiles (t.entid, &t.grideleprof);

				// copy all the file collection to the remote project
				extern void mapfile_copyallfilecollectiontopreferredprojectfolder(void);
				mapfile_copyallfilecollectiontopreferredprojectfolder();
			}
		}

		// 090317 - ignore ebebank new structure to avoid empty EBE icons being added to local library left list
		if (stricmp (t.addentityfile_s.Get(), "..\\ebebank\\_builder\\New Site.fpe") == NULL)
			t.talreadyloaded = 1;
	}
}

void entity_validatearraysize ( void )
{
	//  ensure enough space in entity profile arrays
	if (  g.entidmaster+32>g.entidmastermax ) 
	{
		g.entidmastermax=g.entidmaster+32;
		Dim2(  t.entitybodypart,g.entidmastermax, 100   );
		Dim2(  t.entityappendanim,g.entidmastermax, 100  );
		Dim2(  t.entityanim,g.entidmastermax, g.animmax   );
		Dim2(  t.entityfootfall,g.entidmastermax, g.footfallmax  );
		Dim (  t.entityprofileheader,g.entidmastermax   );
		Dim (  t.entityprofile,g.entidmastermax  );
		Dim2(  t.entitydecal_s,g.entidmastermax, 100  );
		Dim2(  t.entitydecal,g.entidmastermax, 100   );
		g.entitybankmax=g.entidmastermax;
		Dim (  t.entitybank_s,g.entidmastermax  );
	}
}


//PE: GenerateD3D9ForMesh - make sure semantic is stored in old D3D9 format.
//PE: Without get fvf offset can fail , and original skin weight is not used but generated , this can give animation problems.
//PE: This is not a problem when using the importer, as it will save everything in the old D3D9 format into the dbo.
void GenerateD3D9ForMesh(sMesh* pMesh, BOOL bNormals, BOOL bTangents, BOOL bBinormals, BOOL bDiffuse, BOOL bBones)
{
	// get FVF details
	sOffsetMap offsetMap;
	GetFVFValueOffsetMap(pMesh->dwFVF, &offsetMap);

	// deactivate bone flag if no bones in source mesh
	if (pMesh->dwBoneCount == 0) bBones = FALSE;

	// valid mesh (no longer using DXMESH)
	if (pMesh->dwFVF > 0)
	{
		// extract vertex size from mesh
		WORD wNumBytesPerVertex = (WORD)pMesh->dwFVFSize;

		// Starting declaration
		int iDeclarationIndex = 0;
		D3D11_INPUT_ELEMENT_DESC pDeclaration[12];

		// check if mesh already has a component (and build declaration)
		BOOL bHasNormals = FALSE;
		BOOL bHasDiffuse = FALSE;
		BOOL bHasTangents = FALSE;
		BOOL bHasBinormals = FALSE;
		BOOL bHasBlendWeights = FALSE;
		BOOL bHasBlendIndices = FALSE;
		BOOL bHasSecondaryUVs = FALSE;
		if (pMesh->dwFVF & GGFVF_XYZ)
		{
			pDeclaration[iDeclarationIndex].SemanticName = "POSITION";
			pDeclaration[iDeclarationIndex].SemanticIndex = 0;
			pDeclaration[iDeclarationIndex].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			pDeclaration[iDeclarationIndex].InputSlot = 0;
			pDeclaration[iDeclarationIndex].AlignedByteOffset = 0;
			pDeclaration[iDeclarationIndex].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			pDeclaration[iDeclarationIndex].InstanceDataStepRate = 0;
			iDeclarationIndex++;
		}
		if (pMesh->dwFVF & GGFVF_NORMAL)
		{
			pDeclaration[iDeclarationIndex].SemanticName = "NORMAL";
			pDeclaration[iDeclarationIndex].SemanticIndex = 0;
			pDeclaration[iDeclarationIndex].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			pDeclaration[iDeclarationIndex].InputSlot = 0;
			pDeclaration[iDeclarationIndex].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			pDeclaration[iDeclarationIndex].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			pDeclaration[iDeclarationIndex].InstanceDataStepRate = 0;
			iDeclarationIndex++;
			bHasNormals = TRUE;
		}
		if (pMesh->dwFVF & GGFVF_TEX1)
		{
			pDeclaration[iDeclarationIndex].SemanticName = "TEXCOORD";
			pDeclaration[iDeclarationIndex].SemanticIndex = 0;
			pDeclaration[iDeclarationIndex].Format = DXGI_FORMAT_R32G32_FLOAT;
			pDeclaration[iDeclarationIndex].InputSlot = 0;
			pDeclaration[iDeclarationIndex].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			pDeclaration[iDeclarationIndex].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			pDeclaration[iDeclarationIndex].InstanceDataStepRate = 0;
			iDeclarationIndex++;
			bHasDiffuse = TRUE;
		}
		if (pMesh->dwFVF & GGFVF_DIFFUSE)
		{
			pDeclaration[iDeclarationIndex].SemanticName = "COLOR";
			pDeclaration[iDeclarationIndex].SemanticIndex = 0;
			pDeclaration[iDeclarationIndex].Format = DXGI_FORMAT_R32_FLOAT;
			pDeclaration[iDeclarationIndex].InputSlot = 0;
			pDeclaration[iDeclarationIndex].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			pDeclaration[iDeclarationIndex].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			pDeclaration[iDeclarationIndex].InstanceDataStepRate = 0;
			iDeclarationIndex++;
			bHasDiffuse = TRUE;
		}
		if (pMesh->dwFVF & offsetMap.dwTU[1] > 0)
		{
			pDeclaration[iDeclarationIndex].SemanticName = "TEXCOORD";
			pDeclaration[iDeclarationIndex].SemanticIndex = 1;
			pDeclaration[iDeclarationIndex].Format = DXGI_FORMAT_R32G32_FLOAT;
			pDeclaration[iDeclarationIndex].InputSlot = 0;
			pDeclaration[iDeclarationIndex].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			pDeclaration[iDeclarationIndex].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			pDeclaration[iDeclarationIndex].InstanceDataStepRate = 0;
			iDeclarationIndex++;
			bHasSecondaryUVs = TRUE;
		}

		if (!bHasNormals && bNormals)
		{
			pDeclaration[iDeclarationIndex].SemanticName = "NORMAL";
			pDeclaration[iDeclarationIndex].SemanticIndex = 0;
			pDeclaration[iDeclarationIndex].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			pDeclaration[iDeclarationIndex].InputSlot = 0;
			pDeclaration[iDeclarationIndex].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			pDeclaration[iDeclarationIndex].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			pDeclaration[iDeclarationIndex].InstanceDataStepRate = 0;
			iDeclarationIndex++;
			wNumBytesPerVertex += 12;
		}
		if (!bHasDiffuse && bDiffuse)
		{
			pDeclaration[iDeclarationIndex].SemanticName = "COLOR";
			pDeclaration[iDeclarationIndex].SemanticIndex = 0;
			pDeclaration[iDeclarationIndex].Format = DXGI_FORMAT_R32_FLOAT;
			pDeclaration[iDeclarationIndex].InputSlot = 0;
			pDeclaration[iDeclarationIndex].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			pDeclaration[iDeclarationIndex].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			pDeclaration[iDeclarationIndex].InstanceDataStepRate = 0;
			iDeclarationIndex++;
			wNumBytesPerVertex += 4;
		}
		if (!bHasTangents && bTangents)
		{
			pDeclaration[iDeclarationIndex].SemanticName = "TANGENT";
			pDeclaration[iDeclarationIndex].SemanticIndex = 0;
			pDeclaration[iDeclarationIndex].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			pDeclaration[iDeclarationIndex].InputSlot = 0;
			pDeclaration[iDeclarationIndex].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			pDeclaration[iDeclarationIndex].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			pDeclaration[iDeclarationIndex].InstanceDataStepRate = 0;
			iDeclarationIndex++;
			wNumBytesPerVertex += 12;
		}
		if (!bHasBinormals && bBinormals)
		{
			pDeclaration[iDeclarationIndex].SemanticName = "BINORMAL";
			pDeclaration[iDeclarationIndex].SemanticIndex = 0;
			pDeclaration[iDeclarationIndex].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			pDeclaration[iDeclarationIndex].InputSlot = 0;
			pDeclaration[iDeclarationIndex].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			pDeclaration[iDeclarationIndex].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			pDeclaration[iDeclarationIndex].InstanceDataStepRate = 0;
			iDeclarationIndex++;
			wNumBytesPerVertex += 12;
		}
		DWORD dwOffsetToWeights = wNumBytesPerVertex;
		if (!bHasBlendWeights && bBones)
		{
			pDeclaration[iDeclarationIndex].SemanticName = "TEXCOORD";
			pDeclaration[iDeclarationIndex].SemanticIndex = 1;
			pDeclaration[iDeclarationIndex].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			pDeclaration[iDeclarationIndex].InputSlot = 0;
			pDeclaration[iDeclarationIndex].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			pDeclaration[iDeclarationIndex].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			pDeclaration[iDeclarationIndex].InstanceDataStepRate = 0;
			iDeclarationIndex++;
			wNumBytesPerVertex += 16;
		}
		DWORD dwOffsetToIndices = wNumBytesPerVertex;
		if (!bHasBlendIndices && bBones)
		{
			pDeclaration[iDeclarationIndex].SemanticName = "TEXCOORD";
			pDeclaration[iDeclarationIndex].SemanticIndex = 2;
			pDeclaration[iDeclarationIndex].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			pDeclaration[iDeclarationIndex].InputSlot = 0;
			pDeclaration[iDeclarationIndex].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			pDeclaration[iDeclarationIndex].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			pDeclaration[iDeclarationIndex].InstanceDataStepRate = 0;
			iDeclarationIndex++;
			wNumBytesPerVertex += 16;
		}

		// copy declaration into old D3D9 format (as DBO relies on this data in the binary!)
		int iDecIndex = 0;
		int iByteOffset = 0;
		for (; iDecIndex < iDeclarationIndex; iDecIndex++)
		{
			int iEntryByteSize = 0;
			if (stricmp(pDeclaration[iDecIndex].SemanticName, "POSITION") == NULL)
			{
				pMesh->pVertexDeclaration[iDecIndex].Usage = GGDECLUSAGE_POSITION;
				pMesh->pVertexDeclaration[iDecIndex].Type = GGDECLTYPE_FLOAT3;
				iEntryByteSize = 12;
			}
			if (stricmp(pDeclaration[iDecIndex].SemanticName, "NORMAL") == NULL)
			{
				pMesh->pVertexDeclaration[iDecIndex].Usage = GGDECLUSAGE_NORMAL;
				pMesh->pVertexDeclaration[iDecIndex].Type = GGDECLTYPE_FLOAT3;
				iEntryByteSize = 12;
			}
			if (stricmp(pDeclaration[iDecIndex].SemanticName, "COLOR") == NULL)
			{
				pMesh->pVertexDeclaration[iDecIndex].Usage = GGDECLUSAGE_COLOR;
				pMesh->pVertexDeclaration[iDecIndex].Type = GGDECLTYPE_FLOAT2;
				iEntryByteSize = 4;
			}
			if (stricmp(pDeclaration[iDecIndex].SemanticName, "TANGENT") == NULL)
			{
				pMesh->pVertexDeclaration[iDecIndex].Usage = GGDECLUSAGE_TANGENT;
				pMesh->pVertexDeclaration[iDecIndex].Type = GGDECLTYPE_FLOAT3;
				iEntryByteSize = 12;
			}
			if (stricmp(pDeclaration[iDecIndex].SemanticName, "BINORMAL") == NULL)
			{
				pMesh->pVertexDeclaration[iDecIndex].Usage = GGDECLUSAGE_BINORMAL;
				pMesh->pVertexDeclaration[iDecIndex].Type = GGDECLTYPE_FLOAT3;
				iEntryByteSize = 12;
			}
			if (stricmp(pDeclaration[iDecIndex].SemanticName, "TEXCOORD") == NULL)
			{
				pMesh->pVertexDeclaration[iDecIndex].Usage = GGDECLUSAGE_TEXCOORD;
				if (pDeclaration[iDecIndex].Format == DXGI_FORMAT_R32G32B32A32_FLOAT)
				{
					pMesh->pVertexDeclaration[iDecIndex].Type = GGDECLTYPE_FLOAT4;
					iEntryByteSize = 16;
				}
				else
				{
					pMesh->pVertexDeclaration[iDecIndex].Type = GGDECLTYPE_FLOAT2;
					iEntryByteSize = 8;
				}
			}
			pMesh->pVertexDeclaration[iDecIndex].Stream = 0;
			pMesh->pVertexDeclaration[iDecIndex].Method = GGDECLMETHOD_DEFAULT;
			pMesh->pVertexDeclaration[iDecIndex].UsageIndex = pDeclaration[iDecIndex].SemanticIndex;
			pMesh->pVertexDeclaration[iDecIndex].Offset = iByteOffset;
			iByteOffset += iEntryByteSize;
		}
		pMesh->pVertexDeclaration[iDecIndex].Stream = 255;
	}
}

void CreateVectorListFromCPPAssembly ( LPSTR pCCPAssemblyStringLine, std::vector<std::string> & vecOfStrs )
{
	char* pAssemblyString = pCCPAssemblyStringLine;
	if (pAssemblyString)
	{
		// get past equals and any spaces
		while (*pAssemblyString == '=' || *pAssemblyString == 32) pAssemblyString++;

		// now we have the assembly string; adult male hair 01,adult male head 01,adult male body 03,adult male legs 04e,adult male feet 04
		// delimited by a comma, and indicates which parts we used (to specify the textures to copy over)
		char pCustomPathToFolder[MAX_PATH];
		cstr assemblyString_s = FirstToken(pAssemblyString, ",");
		while (assemblyString_s.Len() > 0)
		{
			// work out texture files from this reference, i.e adult male hair 01
			char pAssemblyReference[1024];
			strcpy(pAssemblyReference, assemblyString_s.Get());
			strlwr(pAssemblyReference);
			if (pAssemblyReference[strlen(pAssemblyReference) - 1] == '\n') pAssemblyReference[strlen(pAssemblyReference) - 1] = 0;
			int iBaseCount = g_CharacterType.size();// 3;
			for (int iBaseIndex = 0; iBaseIndex < iBaseCount; iBaseIndex++)
			{
				LPSTR pBaseName = "";
				if (iBaseIndex == 0) pBaseName = "adult male";
				if (iBaseIndex == 1) pBaseName = "adult female";
				if (iBaseIndex == 2) pBaseName = "zombie male";
				if (iBaseIndex == 3) pBaseName = "zombie female";
				if (iBaseIndex > 3)
				{
					pBaseName = g_CharacterType[iBaseIndex].pPartsFolder;
				}
				if (strstr(pAssemblyReference, pBaseName) != NULL)
				{
					// found category
					cstr pPartFolder = "";
					if (iBaseIndex == 0) pPartFolder = "charactercreatorplus\\parts\\adult male\\";
					if (iBaseIndex == 1) pPartFolder = "charactercreatorplus\\parts\\adult female\\";
					if (iBaseIndex == 2) pPartFolder = "charactercreatorplus\\parts\\zombie male\\";
					if (iBaseIndex == 3) pPartFolder = "charactercreatorplus\\parts\\zombie female\\";
					if (iBaseIndex > 3)
					{
						sprintf(pCustomPathToFolder, "charactercreatorplus\\parts\\%s\\", g_CharacterType[iBaseIndex].pPartsFolder);
						pPartFolder = pCustomPathToFolder;
					}

					// add final texture files
					cstr pTmpFile = pPartFolder + pAssemblyReference;
					char pRemoveTag[MAX_PATH];
					strcpy(pRemoveTag, pTmpFile.Get());
					for (int nnn = 0; nnn < strlen(pRemoveTag); nnn++)
					{
						if (pRemoveTag[nnn] == '[')
						{
							if (pRemoveTag[nnn - 1] == ' ') nnn--;
							pRemoveTag[nnn] = 0;
							break;
						}
					}

					// need to strip out the tag [xxx] part to find texture proper
					std::string str = "";
					str = pRemoveTag; str = str + "_color.dds"; vecOfStrs.push_back(str);
					str = pRemoveTag; str = str + "_normal.dds"; vecOfStrs.push_back(str);
					str = pRemoveTag; str = str + "_surface.dds"; vecOfStrs.push_back(str);
					str = pRemoveTag; str = str + "_mask.dds"; vecOfStrs.push_back(str);
					str = pRemoveTag; str = str + "_emissive.dds"; vecOfStrs.push_back(str);
					str = pRemoveTag; str = str + "_roughness.dds"; vecOfStrs.push_back(str);
					str = pRemoveTag; str = str + "_metalness.dds"; vecOfStrs.push_back(str);
					str = pRemoveTag; str = str + "_gloss.dds"; vecOfStrs.push_back(str);
					str = pRemoveTag; str = str + "_ao.dds"; vecOfStrs.push_back(str);
				}
			}
			assemblyString_s = NextToken(",");
		}
	}
}

bool entity_load_thread_prepare(LPSTR pFpeFile)
{
	// preload thread busy, just ignore
	if (object_preload_files_in_progress() || image_preload_files_in_progress())
		return false;

	// vars to do preloading
	cstr sFpeFile = pFpeFile;
	cstr model_s = ""; //Need this from the fpe. t.entityprofile[t.entid].model_s
	int ischaractercreator = 0; //Need this from fpe. t.entityprofile[t.entid].ischaractercreator
	cstr sFile = "", sDboFile = "";
	std::vector<std::string> fpe_file;

	// work out entity bank path
	cstr sEntityBank = "entitybank\\";
	if (cstr(Lower(Left(sFpeFile.Get(), 11))) == "entitybank\\")
	{
		sFpeFile = Right(sFpeFile.Get(), Len(sFpeFile.Get()) - 11);
	}
	if (cstr(Lower(Left(sFpeFile.Get(), 8))) == "ebebank\\")
	{
		sEntityBank = "";
	}

	// already have this loaded into the level
	for (t.t = 1; t.t <= g.entidmaster; t.t++)
	{
		if (t.entitybank_s[t.t] == sFpeFile)
		{
			// fpe already loaded ready to use
			return true; 
		}
	}

	// find the entity file
	cstr epath_s = getpath(sFpeFile.Get());
	sFpeFile = sEntityBank + sFpeFile;
	if (FileExist(sFpeFile.Get()) == 0)
	{
		return(false); //FPE not found.
	}

	if (getVectorFileContent(sFpeFile.Get(), fpe_file, true) == false)
	{
		return(false); //FPE not found.
	}

	// find model file (DBO)
	std::string model = GetLineParameterFromVectorFile("model=", fpe_file, true);
	if (model.length() <= 0) return(false);

	// determine if character creator object
	std::string sIscharacterCreator = GetLineParameterFromVectorFile("charactercreator=", fpe_file, true);
	model_s = model.c_str();
	if (sIscharacterCreator.length() > 0 && atoi(sIscharacterCreator.c_str()) == 1)
		ischaractercreator = 1;

	// check if we can preload this model
	if (ischaractercreator == 0)
		sFile = sEntityBank + epath_s + model_s;
	else
		sFile = model_s;

	// if .X or .FBX file specified, and DBO exists, load DBO as main model file
	int iSrcFormat = 0;
	if (strcmp(Lower(Right(sFile.Get(), 2)), ".x") == 0) iSrcFormat = 1;
	if (strcmp(Lower(Right(sFile.Get(), 4)), ".fbx") == 0) iSrcFormat = 2;
	if (iSrcFormat > 0)
	{
		if (iSrcFormat == 1)
		{
			// X File Format
			sDboFile = Left(sFile.Get(), Len(sFile.Get()) - 2); sDboFile += ".dbo";
			if (sDboFile != "" && FileExist(sDboFile.Get()) == 1) sFile = sDboFile;
		}
		if (iSrcFormat == 2)
		{
			// FBX File Format
			sDboFile = Left(sFile.Get(), Len(sFile.Get()) - 4); sDboFile += ".dbo";
			if (sDboFile != "" && FileExist(sDboFile.Get()) == 1) sFile = sDboFile;
		}
	}
	else
	{
		// if .X or .FBX file NOT specified
		sDboFile = "";
		if (strcmp(Lower(Right(sFile.Get(), 4)), ".dbo") == 0)
		{
			if (FileExist(sFile.Get()) == 0)
			{
				//We can only preload dbo files.
				return(false);
			}
		}
	}

	// get path to original model
	char pModelPath[10248];
	strcpy(pModelPath, "");
	LPSTR pOrigModelFilename = sFile.Get();
	for (int n = strlen(pOrigModelFilename); n > 0; n--)
	{
		if (pOrigModelFilename[n] == '\\' || pOrigModelFilename[n] == '/')
		{
			strcpy(pModelPath, pOrigModelFilename);
			pModelPath[n + 1] = 0;
			break;
		}
	}
	if (FileExist(sFile.Get()) == 0)
	{
		sFile = model_s;
		if (strcmp(Lower(Right(sFile.Get(), 2)), ".x") == 0) { sDboFile = Left(sFile.Get(), Len(sFile.Get()) - 2); sDboFile += ".dbo"; }
		else sDboFile = "";
		if (sDboFile != "" && FileExist(sDboFile.Get()) == 1)  sFile = sDboFile;
	}

	// if DBO model file exists
	if (FileExist(sFile.Get()) == 1)
	{
		if (FileExist(sDboFile.Get()) == 1)
		{
			sFile = sDboFile;
		}
		if (strcmp(Lower(Right(sFile.Get(), 4)), ".dbo") == 0)
		{
			// also scan FPE for all references to textures and preload those
			image_preload_files_start();
			for (int iTexLineScan = 0; iTexLineScan < 100; iTexLineScan++)
			{
				// texture ref field name
				char pTexRefFieldNam[32];
				if ( iTexLineScan > 0 ) 
					sprintf (pTexRefFieldNam, "textureref%d=", iTexLineScan);
				else
					strcpy (pTexRefFieldNam, "textured=");

				// get texture ref data from FPE file
				std::string texturefileref = "";
				texturefileref = GetLineParameterFromVectorFile(pTexRefFieldNam, fpe_file, true);

				// if texture ref valid
				if (texturefileref.length() > 0)
				{
					// certainly preload the one specified
					LPSTR pTextureRef = (LPSTR)texturefileref.c_str();

					// add to image preload
					char pPreloadThisFile[MAX_PATH];
					strcpy (pPreloadThisFile, pModelPath);
					strcat (pPreloadThisFile, pTextureRef);
					image_preload_files_add(pPreloadThisFile);

					// clean tex ref and remove extension if any
					char pCleanTexRef[MAX_PATH];
					strcpy(pCleanTexRef, pTextureRef);
					strlwr(pCleanTexRef);
					LPSTR pExt = NULL;
					pExt = strstr(pCleanTexRef, ".dds"); if (pExt) *pExt = 0;
					pExt = strstr(pCleanTexRef, ".png"); if (pExt) *pExt = 0;
					pExt = strstr(pCleanTexRef, ".tga"); if (pExt) *pExt = 0;
					pExt = strstr(pCleanTexRef, ".jpg"); if (pExt) *pExt = 0;
					pExt = strstr(pCleanTexRef, ".bmp"); if (pExt) *pExt = 0;

					// preload all its PBR set variants
					if ( strstr (pCleanTexRef, "_color") != NULL)
					{
						char pBaseTexRef[MAX_PATH];
						strcpy (pBaseTexRef, pCleanTexRef);
						pBaseTexRef[strlen(pBaseTexRef) - 6] = 0;
						for (int iPBRSet = 1; iPBRSet < 5; iPBRSet++)
						{
							char pThisPBRTex[MAX_PATH];
							strcpy (pThisPBRTex, pBaseTexRef);
							if (iPBRSet == 1) strcat (pThisPBRTex, "_normal.dds");
							if (iPBRSet == 2) strcat (pThisPBRTex, "_surface.dds");
							if (iPBRSet == 3) strcat (pThisPBRTex, "_emissive.dds");
							if (iPBRSet == 4) strcat (pThisPBRTex, "_displacement.dds");
							strcpy (pPreloadThisFile, pModelPath);
							strcat (pPreloadThisFile, pThisPBRTex);
							image_preload_files_add(pPreloadThisFile);
						}
					}
				}
				// also account for character creator assemblies
				std::vector<std::string> ccpAsemblyList;
				std::string ccpAssemblyRef = GetLineParameterFromVectorFile("ccpassembly=", fpe_file, true);
				CreateVectorListFromCPPAssembly((LPSTR)ccpAssemblyRef.c_str(), ccpAsemblyList);
				for ( int i = 0; i < ccpAsemblyList.size(); i++ ) image_preload_files_add((LPSTR)ccpAsemblyList[i].c_str());
				ccpAsemblyList.clear();
			}
			image_preload_files_finish();

			// preload is a go!
			return(true);
		}
	}

	// otherwise not preloading today
	return(false);
}

bool entity_load (bool bCalledFromLibrary)
{
	// allowed by default on successful load of the model
	bool bSavingDBOAllowed = true;

	//  Activate auto generation of mipmaps for ALL entities
	SetImageAutoMipMap (  1 );

	//  Entity Object Index
	t.entobj=g.entitybankoffset+t.entid;

	//  debug info
	t.mytimer=Timer();

	//  Load Entity profile data
	entity_loaddata ();

	// special group detection
	bool bWeAreAGroup = false;
	if (g_iAbortedAsEntityIsGroupFileMode == 3)
	{
		// these entities are loaded by the group (store which group this child belongs)
		extern int current_selected_group;
		t.entityprofile[t.entid].ischildofgroup = 1 + current_selected_group;
	}
	cstr LastGroupFilename_s = "";
	if (g_iAbortedAsEntityIsGroupFileMode == 2)
	{
		// okay we are a group!
		int iStoreObj = t.entobj;
		int iStoreEntID = t.entid;
		extern bool LoadGroup(LPSTR pAbsFilename);
		cstr pGroupFilename_s = t.tFPEName_s;
		LastGroupFilename_s = pGroupFilename_s;
		if (bCalledFromLibrary)
		{
			//PE: check if we already got a thumb.
			//PE: Check if a got the original image for this smart object.
			bool CreateBackBufferCacheName(char *file, int width, int height);
			extern cstr BackBufferCacheName;
			CreateBackBufferCacheName(t.addentityfile_s.Get(), 512, 288);
			extern StoryboardStruct Storyboard;
			if (!(strlen(Storyboard.gamename) > 0 && strlen(Storyboard.customprojectfolder) > 0))
			{
				GG_SetWritablesToRoot(true);
			}
			if (FileExist(BackBufferCacheName.Get()))
			{
				LoadImage((char *)BackBufferCacheName.Get(), ENTITY_CACHE_ICONS_LARGE + t.entid);
				if (ImageExist(ENTITY_CACHE_ICONS_LARGE + t.entid))
				{
					t.entityprofile[t.entid].iThumbnailLarge = ENTITY_CACHE_ICONS_LARGE + t.entid;
					g_iAbortedAsEntityIsGroupFileMode = 0;
					//PE: No need to read the group and load the files we have the thumb.
					GG_SetWritablesToRoot(false);
					return(false);
				}
			}
			GG_SetWritablesToRoot(false);
		}
		
		g_iAbortedAsEntityIsGroupFileMode = 3;
		LoadGroup(pGroupFilename_s.Get());
		g_iAbortedAsEntityIsGroupFileMode = 0;

		t.entobj = iStoreObj;
		t.entid = iStoreEntID;
		bWeAreAGroup = true;
		// after group created, need to clear reminants from cursor
		g.thumbentityrubberbandlist = g.entityrubberbandlist;
		g.entityrubberbandlist.clear();

		//Make sure any selection are removed
		t.gridentity = 0;
		t.inputsys.constructselection = 0;
		t.inputsys.domodeentity = 1;
		t.grideditselect = 5;
		editor_refresheditmarkers();
	}

	//  Only load characters for entity-local-testing
	t.desc_s=t.entityprofileheader[t.entid].desc_s;
	if (  t.scanforentitiescharactersonly == 1 ) 
	{
		if (  t.entityprofile[t.entid].ischaracter == 0 ) 
		{
			t.desc_s="";
		}
	}

	//  Special mode (from lightmapper) which only loads static entities
	//  which will speed up lightmapping process and reduce hit on system memory
	//  not excluding markers as we need some of them for lighting info
	if (  t.lightmapper.onlyloadstaticentitiesduringlightmapper == 1 ) 
	{
		if (  t.entityprofile[t.entid].ischaracter == 1 ) 
		{
			t.desc_s="";
		}
	}

	// Only if profile data exists
	if (t.desc_s != "")
	{
		//Load the thumbnail.
		#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 
		t.strwork = t.entdir_s + t.ent_s;
		t.tthumbbmpfile_s = "";	t.tthumbbmpfile_s = t.tthumbbmpfile_s + Left(t.strwork.Get(), (Len(t.entdir_s.Get()) + Len(t.ent_s.Get())) - 4) + ".bmp";

		GG_SetWritablesToRoot(true);
		image_setlegacyimageloading(true);
		LoadImage(t.tthumbbmpfile_s.Get(), ENTITY_CACHE_ICONS + t.entid);
		image_setlegacyimageloading(false);
		GG_SetWritablesToRoot(false);
		t.entityprofile[t.entid].iThumbnailSmall = ENTITY_CACHE_ICONS + t.entid;
		if (!ImageExist(t.entityprofile[t.entid].iThumbnailSmall))
			t.entityprofile[t.entid].iThumbnailSmall = TOOL_ENTITY;
		t.strwork = t.entdir_s + t.ent_s;
		if (ImageExist(t.entityprofile[t.entid].iThumbnailLarge))
		{
			// ensure the latest thumb is used
			DeleteImage(t.entityprofile[t.entid].iThumbnailLarge);
		}
		//t.entityprofile[t.entid].iThumbnailLarge = 0; and do not wipe out necessary index for the eventual reload
		bool CreateBackBufferCacheName(char *file, int width, int height);
		extern cstr BackBufferCacheName;

		SetMipmapNum(1); //PE: mipmaps not needed.
		image_setlegacyimageloading(true);

		bool bWeGotaThumb = false;
		if (bWeAreAGroup)
		{
			//PE: Check if a got the original image for this smart object.
			CreateBackBufferCacheName(LastGroupFilename_s.Get(), 512, 288);
			extern StoryboardStruct Storyboard;
			if (!(strlen(Storyboard.gamename) > 0 && strlen(Storyboard.customprojectfolder) > 0))
			{
				GG_SetWritablesToRoot(true);
			}
			if (FileExist(BackBufferCacheName.Get()))
			{
				LoadImage((char *)BackBufferCacheName.Get(), ENTITY_CACHE_ICONS_LARGE + t.entid);
				if (ImageExist(ENTITY_CACHE_ICONS_LARGE + t.entid))
				{
					t.entityprofile[t.entid].iThumbnailLarge = ENTITY_CACHE_ICONS_LARGE + t.entid;
					bWeGotaThumb = true;
				}
			}
			GG_SetWritablesToRoot(false);
		}

		if (!bWeGotaThumb)
		{
			CreateBackBufferCacheName(t.strwork.Get(), 512, 288);
			extern StoryboardStruct Storyboard;
			if (!(strlen(Storyboard.gamename) > 0 && strlen(Storyboard.customprojectfolder) > 0))
			{
				GG_SetWritablesToRoot(true);
			}
			if (FileExist(BackBufferCacheName.Get()))
			{
				LoadImage((char *)BackBufferCacheName.Get(), ENTITY_CACHE_ICONS_LARGE + t.entid);
				if (ImageExist(ENTITY_CACHE_ICONS_LARGE + t.entid))
					t.entityprofile[t.entid].iThumbnailLarge = ENTITY_CACHE_ICONS_LARGE + t.entid;
			}
			GG_SetWritablesToRoot(false);
		}
		image_setlegacyimageloading(false);
		SetMipmapNum(-1);
		#endif

		// if a group, make a modified entity that references a group
		if (bWeAreAGroup == true)
		{
			// the group entity
			int iGroupEntID = t.entid;
			int iGroupEntObj = t.entobj;

			// if a 'group' entity, set reference to group
			extern int current_selected_group;
			t.entityprofile[iGroupEntID].model_s = "group";

			// record this group into entity parent profile
			t.entityprofile[iGroupEntID].groupreference = 1; // group indexes can MOVE!

			// test object!
			g_iWickedEntityId = iGroupEntID;
			MakeObjectCube(iGroupEntObj, 0.0f);
			g_iWickedEntityId = -1;

			// this is ther group object entity ID!
			t.entid = iGroupEntID;
			t.entobj = iGroupEntObj;

			//PE: Moved here so we get the groupreference.
			//PE: current_selected_group is used by thumb system , and could crash without it.
			extern int thumb_selected_group;
			thumb_selected_group = current_selected_group;
			current_selected_group = -1; // fixes cellar issue sticky candle and switch escape crash
			// and go no further
			return false;
		}

		//  Load the model
		if (t.entityprofile[t.entid].ischaractercreator == 0)
		{
			t.tfile_s = t.entdir_s + t.entpath_s + t.entityprofile[t.entid].model_s;
		}
		else
		{
			t.tfile_s = t.entityprofile[t.entid].model_s;
		}
		deleteOutOfDateDBO(t.tfile_s.Get());
		// if .X or .FBX file specified, and DBO exists, load DBO as main model file
		int iSrcFormat = 0;
		if (strcmp(Lower(Right(t.tfile_s.Get(), 2)), ".x") == 0) iSrcFormat = 1;
		if (strcmp(Lower(Right(t.tfile_s.Get(), 4)), ".fbx") == 0) iSrcFormat = 2;
		if (iSrcFormat > 0)
		{
			if (iSrcFormat == 1)
			{
				// X File Format
				t.tdbofile_s = Left(t.tfile_s.Get(), Len(t.tfile_s.Get()) - 2); t.tdbofile_s += ".dbo";
				if (t.tdbofile_s != "" && FileExist(t.tdbofile_s.Get()) == 1) t.tfile_s = t.tdbofile_s;
			}
			if (iSrcFormat == 2)
			{
				// FBX File Format
				t.tdbofile_s = Left(t.tfile_s.Get(), Len(t.tfile_s.Get()) - 4); t.tdbofile_s += ".dbo";
				if (t.tdbofile_s != "" && FileExist(t.tdbofile_s.Get()) == 1) t.tfile_s = t.tdbofile_s;
			}
		}
		else
		{
			// if .X or .FBX file NOT specified
			t.tdbofile_s = "";
			if (strcmp(Lower(Right(t.tfile_s.Get(), 4)), ".dbo") == 0)
			{
				// and .DBO specified instead, check if .DBO does not exist
				if (FileExist(t.tfile_s.Get()) == 0)
				{
					// if not exist try .X model file (typical of model import entities that use original X file)
					// but do not copy the DBO file with it
					t.tfile_s = Left(t.tfile_s.Get(), Len(t.tfile_s.Get()) - 4); t.tfile_s += ".x";
				}
			}
		}
		// get path to original model
		char pModelPath[10248];
		strcpy(pModelPath, "");
		LPSTR pOrigModelFilename = t.tfile_s.Get();
		for (int n = strlen(pOrigModelFilename); n > 0; n--)
		{
			if (pOrigModelFilename[n] == '\\' || pOrigModelFilename[n] == '/')
			{
				strcpy(pModelPath, pOrigModelFilename);
				pModelPath[n + 1] = 0;
				break;
			}
		}
		// 070718 - if append final file exists, use that
		bool bUsingAppendAnimFileModel = false;
		cstr pAppendFinalModelFilename = t.entityappendanim[t.entid][0].filename;
		if (strlen(pAppendFinalModelFilename.Get()) > 0)
		{
			pAppendFinalModelFilename = cstr(pModelPath) + pAppendFinalModelFilename;
			if (FileExist(pAppendFinalModelFilename.Get()) == 1)
			{
				bUsingAppendAnimFileModel = true;
				t.tfile_s = pAppendFinalModelFilename;
				pAppendFinalModelFilename = "";
				t.tdbofile_s = "";
			}
		}
		if (FileExist(t.tfile_s.Get()) == 0)
		{
			t.tfile_s = t.entityprofile[t.entid].model_s;
			//  V109 BETA6 - 290408 - allow DBO creation/read if full relative path provides (i.e. meshbank\scifi\etc)
			if (strcmp(Lower(Right(t.tfile_s.Get(), 2)), ".x") == 0) { t.tdbofile_s = Left(t.tfile_s.Get(), Len(t.tfile_s.Get()) - 2); t.tdbofile_s += ".dbo"; }
			else t.tdbofile_s = "";
			if (t.tdbofile_s != "" && FileExist(t.tdbofile_s.Get()) == 1)  t.tfile_s = t.tdbofile_s;
		}
		t.txfile_s = t.tfile_s;
		if (FileExist(t.tfile_s.Get()) == 1)
		{
			//  if DBO version exists, use that (quicker)
			if (FileExist(t.tdbofile_s.Get()) == 1)
			{
				t.tfile_s = t.tdbofile_s;
				t.tdbofile_s = "";
			}
			else
			{
				//  allowed to save DBO (once only)
			}

			bool bNewDecal = false;
			bool bLODLoaded = false;
			//  Load entity (compile does not need the dynamic objects)
			if (t.entobj > 0)
			{
				if (ObjectExist(t.entobj) == 0)
				{
					extern int g_iUseLODObjects;
					extern bool bDisableLODLoad;
					if (g_iUseLODObjects > 0 && !bDisableLODLoad)
					{
						//PE: Try to locate a LOD object.
						std::string lodname = t.tFPEName_s.Get();
						replaceAll(lodname, ".fpe", "_lod.dbo");

						if (FileExist( (char *) lodname.c_str()))
						{
							//PE: Make sure LOD use the correct fpe settings.
							g_iWickedEntityId = t.entid;
							LoadObject((char *) lodname.c_str(), t.entobj);
							if (ObjectExist(t.entobj))
							{
								bLODLoaded = true;
							}
							g_iWickedEntityId = -1;
						}
					}
					// load entity model
					g_iWickedEntityId = t.entid;
					char debug[ 1024 ];
					sprintf(debug, "LoadObject( %s )", t.tfile_s.Get());
					timestampactivity(0, debug);
							
					char* tfile_s = t.tfile_s.Get();
					if (!bLODLoaded)
					{
						if (*tfile_s) LoadObject(t.tfile_s.Get(), t.entobj);
						else MakeObjectBox(t.entobj, 1, 1, 1);
					}
					g_iWickedEntityId = -1;

					if (ObjectExist(t.entobj) == 0)
					{
						// soft error allows failed object loads to continue (message provided in LoadDBO)
						MakeObjectCube(t.entobj, 25);

						// and prevent this temp shape saving as permanent DBO!
						bSavingDBOAllowed = false;
					}

					//LB: default position is not a decal (fixes issue of objects being misaligned in object library auto gen)
					t.entityprofile[t.entid].bIsDecal = false;
					//PE: Old decal support.
					cstr sEffectLower = Lower(t.entityprofile[t.entid].effect_s.Get());
					if (sEffectLower == "effectbank\\reloaded\\decal_animate4.fx")
					{
						t.entityprofile[t.entid].bIsDecal = true;
						t.entityprofile[t.entid].iDecalRows = 4;
						t.entityprofile[t.entid].iDecalColumns = 4;
						t.entityprofile[t.entid].fDecalSpeed = 1.0; // ? later
						bNewDecal = true;
					}
					if (sEffectLower == "effectbank\\reloaded\\decal_animate8.fx")
					{
						t.entityprofile[t.entid].bIsDecal = true;
						t.entityprofile[t.entid].iDecalRows = 8;
						t.entityprofile[t.entid].iDecalColumns = 8;
						t.entityprofile[t.entid].fDecalSpeed = 1.0; // ? later
						bNewDecal = true;
					}
					if (sEffectLower == "effectbank\\reloaded\\decal_animate20.fx")
					{
						t.entityprofile[t.entid].bIsDecal = true;
						t.entityprofile[t.entid].iDecalRows = 20;
						t.entityprofile[t.entid].iDecalColumns = 20;
						t.entityprofile[t.entid].fDecalSpeed = 1.0; // ? later
						bNewDecal = true;
					}
					if (sEffectLower == "effectbank\\reloaded\\decal_animate10.fx" || sEffectLower == "effectbank\\reloaded\\decal_animate10x10.fx")
					{
						t.entityprofile[t.entid].bIsDecal = true;
						t.entityprofile[t.entid].iDecalRows = 10;
						t.entityprofile[t.entid].iDecalColumns = 10;
						t.entityprofile[t.entid].fDecalSpeed = 1.0; // ? later
						bNewDecal = true;
					}
					if (sEffectLower == "effectbank\\reloaded\\decal_animate10x12.fx")
					{
						t.entityprofile[t.entid].bIsDecal = true;
						t.entityprofile[t.entid].iDecalRows = 10;
						t.entityprofile[t.entid].iDecalColumns = 12;
						t.entityprofile[t.entid].fDecalSpeed = 1.0; // ? later
						bNewDecal = true;
					}
					if (bNewDecal)
					{
						float x = ObjectSizeX(t.entobj);
						float y = ObjectSizeY(t.entobj);
						float z = ObjectSizeZ(t.entobj);
						if (ObjectExist(t.entobj)) DeleteObject(t.entobj);
						MakeObjectPlane(t.entobj, x, y);
						PositionObject(t.entobj, 0, y, 0);
						FixObjectPivot(t.entobj);
						SetAlphaMappingOn(t.entobj, 100.0);
						SetObjectTransparency(t.entobj, 6);

						LockVertexDataForLimbCore(t.entobj, 0, 1);
						SetVertexDataNormals(0, 0, 1, 0);
						SetVertexDataNormals(1, 0, 1, 0);
						SetVertexDataNormals(2, 0, 1, 0);
						SetVertexDataNormals(3, 0, 1, 0);
						SetVertexDataNormals(4, 0, 1, 0);
						SetVertexDataNormals(5, 0, 1, 0);
						UnlockVertexData();

						SetObjectUVManually(t.entobj, 0, t.entityprofile[t.entid].iDecalRows , t.entityprofile[t.entid].iDecalColumns );

						SetObjectLight(t.entobj, 0);

						void SetupDecalObject(int obj,int elementID);
						SetupDecalObject(t.entobj,0);

					}

					// 060718 - append animation data from other DBO files
					if (bUsingAppendAnimFileModel == false)
					{
						if (Len(t.tdbofile_s.Get()) == 0)
						{
							if (t.entityprofile[t.entid].appendanimmax > 0)
							{
								for (int aa = 1; aa <= t.entityprofile[t.entid].appendanimmax; aa++)
								{
									cstr pAppendModelFilename = cstr(pModelPath) + t.entityappendanim[t.entid][aa].filename;
									int iStartFrame = t.entityappendanim[t.entid][aa].startframe;
									AppendObject(pAppendModelFilename.Get(), t.entobj, iStartFrame);
								}
							}
						}
					}

					//PE: Only if not already set by a wicked material.
					if (!t.entityprofile[t.entid].WEMaterial.MaterialActive && t.entityprofile[t.entid].WEMaterial.dwBaseColor[0] == -1)
						SetObjectDiffuse(t.entobj, Rgb(255, 255, 255));

					// wipe ANY material emission colors
					if (!t.entityprofile[t.entid].WEMaterial.MaterialActive)
					{
						// only if NOT using an active Wicked material (so multi-mesh object thumbs and previews can have emissives!)
						SetObjectEmissive(t.entobj, 0);
					}

					// prepare properties
					SetFastBoundsCalculation(1);
					SetObjectFilter(t.entobj, 2);
					SetObjectCollisionOff(t.entobj);

					//  if strictly NON-multimaterial, convert now
					if (t.entityprofile[t.entid].onetexture == 1)
					{
						if (GetMeshExist(g.meshgeneralwork) == 1)  DeleteMesh(g.meshgeneralwork);
						MakeMeshFromObject(g.meshgeneralwork, t.entobj);
						DeleteObject(t.entobj);
						MakeObject(t.entobj, g.meshgeneralwork, 0);
					}

					// 011215 - if specified, we can smooth the model before we use it (concrete pipe in TBE level)
					float fSmoothingAngleOrFullGenerate = t.entityprofile[t.entid].smoothangle;
					if (fSmoothingAngleOrFullGenerate > 0 && fSmoothingAngleOrFullGenerate <= 200)
					{
						// 090217 - this only works on orig X files (not subsequent DBO) as they change 
						// the mesh which is then saved out (below)
						if (Len(t.tdbofile_s.Get()) > 1)
						{
							// 090216 - a special mode of over 101 will flip normals for the object (when normals are bad)
							if (fSmoothingAngleOrFullGenerate >= 101.0f)
							{
								SetObjectNormalsEx(t.entobj, 0); // will generate new smooth normals for object
								fSmoothingAngleOrFullGenerate -= 101.0f;
							}

							// and if smoothing factor required, weld some of them together
							if (fSmoothingAngleOrFullGenerate > 0.0f)
							{
								SetObjectSmoothing(t.entobj, fSmoothingAngleOrFullGenerate);
							}
						}
						else
						{
							// some support for direct DBO model smoothing 
							if (fSmoothingAngleOrFullGenerate > 0.0f)
							{
								SetObjectSmoothing(t.entobj, fSmoothingAngleOrFullGenerate);
							}
						}
					}
					else
					{
						// smooth any model, not just X files going to DBO files
						if (fSmoothingAngleOrFullGenerate >= 201)
						{
							SetObjectNormalsEx(t.entobj, 0);
							fSmoothingAngleOrFullGenerate -= 201.0f;
							SetObjectSmoothing(t.entobj, fSmoothingAngleOrFullGenerate);
						}
					}
				}
			}

			//  loaded okay
			if (ObjectExist(t.entobj) == 1)
			{
				// 070718 - if append final model needs to be created, prefer that
				if (strlen(pAppendFinalModelFilename.Get()) > 0)
					if (FileExist(pAppendFinalModelFilename.Get()) == 0)
						t.tdbofile_s = pAppendFinalModelFilename;

				// Save if DBO not exist for entity (for fast loading)
				if (Len(t.tdbofile_s.Get()) > 1 && bSavingDBOAllowed == true && !bLODLoaded )
				{
					// ensure legacy compatibility (avoids new mapedito crashing build process)
					// in wicked, only save if not exist, otherwise existing DBO is not to be touched!
					if (FileExist(t.tdbofile_s.Get()) == 0) SaveObject(t.tdbofile_s.Get(), t.entobj);
					if (FileExist(t.tdbofile_s.Get()) == 1)
					{
						DeleteObject(t.entobj);
						LoadObject(t.tdbofile_s.Get(), t.entobj);
						SetObjectFilter(t.entobj, 2);
						SetObjectCollisionOff(t.entobj);
					}
				}

				// 300817 - if an EBE object with no .EBE file, remove handle from entity
				if (t.entityprofile[t.entid].isebe == 2)
					if (ObjectExist(t.entobj) == 1)
						if (LimbExist(t.entobj, 0) == 1)
							ChangeMesh(t.entobj, 0, 0);

				// Special matrix transform mode for FBX and similar models
				SetObjectRenderMatrixMode(t.entobj, t.entityprofile[t.entid].matrixmode);

				//PE: We are missing skin weight/others in old DX9 DBO setup. needed for some functions.
				//PE: prevent generation of vertex weight that screw up some animations.
				sObject* pObject = g_ObjectList[t.entobj];
				for (int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++)
				{
					sMesh* pMesh = pObject->ppMeshList[iMesh];
					GenerateD3D9ForMesh(pMesh, true, true, true, true, true);
				}

				// 131115 - fixes issue of some models not being able to detect with intersectall
				SetObjectDefAnim(t.entobj, t.entityprofile[t.entid].ignoredefanim);

				//  the reverse can be used to allow transparent limbs to be rendered last
				if (t.entityprofile[t.entid].reverseframes == 1)
				{
					ReverseObjectFrames(t.entobj);
				}

				//  main profile object adjustments
				RemoveFixedScale(t.entobj);

				if (t.entityprofile[t.entid].scale != 0)
				{
					ScaleObject(t.entobj, t.entityprofile[t.entid].scale, t.entityprofile[t.entid].scale, t.entityprofile[t.entid].scale);
					//PE: Particle scale bug fix.
					if (t.entityprofile[t.entid].ismarker == 10)
					{
						//PE: Particle use fixed scale.
						SetFixedScale(t.entobj, t.entityprofile[t.entid].scale, t.entityprofile[t.entid].scale, t.entityprofile[t.entid].scale);
					}
				}


				//  Apply texture and effect to entity profile obj
				entity_loadtexturesandeffect();

				//  until static bonemodel scales when not animating, loop
				if (t.entityprofile[t.entid].ischaracter == 1)
				{
					//  refresh using loop stop to get correct character pose
					LoopObject(t.entobj); StopObject(t.entobj);
					//  pivot character to face right way
					RotateObject(t.entobj, 0, 180, 0); FixObjectPivot(t.entobj);
				}

				//if (!bNewDecal)
				{
					//  if entity uses a handle, create and attach it now
					t.entityprofile[t.entid].addhandlelimb = 0;
					if (Len(t.entityprofile[t.entid].addhandle_s.Get()) > 1)
					{
						t.thandlefile_s = t.entdir_s + t.entpath_s + "\\" + t.entityprofile[t.entid].addhandle_s;
						if (FileExist(t.thandlefile_s.Get()) == 1)
						{
							if (ObjectExist(g.entityworkobjectoffset) == 1)  DeleteObject(g.entityworkobjectoffset);
							WickedCall_PresetObjectRenderLayer(GGRENDERLAYERS_CURSOROBJECT);
							MakeObjectPlane(g.entityworkobjectoffset, 25, 25);
							WickedCall_PresetObjectRenderLayer(GGRENDERLAYERS_NORMAL);
							RotateObject(g.entityworkobjectoffset, 90, 0, 0);
							ScaleObject(g.entityworkobjectoffset, 50, 50, 50);

							if (GetMeshExist(g.entityworkobjectoffset) == 1)  DeleteMesh(g.entityworkobjectoffset);
							MakeMeshFromObject(g.entityworkobjectoffset, g.entityworkobjectoffset);

							if (bNewDecal)
							{
								//PE: OffsetLimb dont work in wicked , so... (bIsDecal)
								t.tyoffset_f = (t.entityprofile[t.entid].defaultheight / ((t.entityprofile[t.entid].scale + 0.0) / 100.0))*-1;
								LockVertexDataForMesh(g.entityworkobjectoffset);
								for (int i = 0; i < GetVertexDataVertexCount(); i++)
								{
									float vertX = GetVertexDataPositionX(i);
									float vertY = GetVertexDataPositionY(i);
									float vertZ = GetVertexDataPositionZ(i);
									SetVertexDataPosition(i, vertX, vertY + (2 + t.tyoffset_f), vertZ);
								}

								float rc = 4.0;
								SetVertexDataUV(0, 0, 0);
								SetVertexDataUV(1, 1.0 / rc, 0);
								SetVertexDataUV(2, 1.0 / rc, 1.0 / rc);
								SetVertexDataUV(3, 1.0 / rc, 1.0 / rc);
								SetVertexDataUV(4, 0, 1.0 / rc);
								SetVertexDataUV(5, 0, 0);

								UnlockVertexData();
							}

							PerformCheckListForLimbs(g.entityworkobjectoffset);
							if (bNewDecal)
								t.entityprofile[t.entid].addhandlelimb = 1;
							else
								t.entityprofile[t.entid].addhandlelimb = 1 + ChecklistQuantity();

							AddLimb(t.entobj, t.entityprofile[t.entid].addhandlelimb, g.entityworkobjectoffset);
							if (ConfirmObjectAndLimb(t.entobj, t.entityprofile[t.entid].addhandlelimb))
							{
								LinkLimb(t.entobj, 0, t.entityprofile[t.entid].addhandlelimb);
								t.tyoffset_f = (t.entityprofile[t.entid].defaultheight / ((t.entityprofile[t.entid].scale + 0.0) / 100.0))*-1;
								//PE: Offset already done.
								if (!bNewDecal)
									OffsetLimb(t.entobj, t.entityprofile[t.entid].addhandlelimb, 0, 2 + t.tyoffset_f, 0);
								t.texhandleid = loadinternaltexture(t.thandlefile_s.Get());
								TextureLimb(t.entobj, t.entityprofile[t.entid].addhandlelimb, t.texhandleid);
								//SetLimbEffect(t.entobj, t.entityprofile[t.entid].addhandlelimb, t.entityprofile[t.entid].usingeffect);
								if (ObjectExist(g.entityworkobjectoffset) == 1)  DeleteObject(g.entityworkobjectoffset);
								if (GetMeshExist(g.entityworkobjectoffset) == 1)  DeleteMesh(g.entityworkobjectoffset);
								if (t.game.gameisexe == 1)
									HideLimb(t.entobj, t.entityprofile[t.entid].addhandlelimb);
							}
						}
					}
				}

				//  Parent LOD is not enabled, we use clone and instance
				//  settings as we need per-entity element distance control
				if (t.entityprofile[t.entid].isebe == 0)
				{
					// skip if material should come direct from DBO mesh data
					if (t.entityprofile[t.entid].materialindex != 99999)
					{
						// only set material if not EBE, as EBE objects carry per-mesh material values
						SetObjectArbitaryValue(t.entobj, t.entityprofile[t.entid].materialindex);
					}
				}

				//  if entity has decals, find indexes to decals (which are already preloaded)
				t.entityprofile[t.entid].bloodscorch = 0;
				if (t.entityprofile[t.entid].decalmax > 0)
				{
					for (t.tq = 0; t.tq <= t.entityprofile[t.entid].decalmax - 1; t.tq++)
					{
						t.decal_s = t.entitydecal_s[t.entid][t.tq];
						if (strcmp(Lower(t.decal_s.Get()), "blood") == 0)  t.entityprofile[t.entid].bloodscorch = 1;
						decal_find();
						if (t.decalid < 0)  t.decalid = 0;
						t.entitydecal[t.entid][t.tq] = t.decalid;
					}
				}

				//  HideObject (  away )
				PositionObject(t.entobj, 100000, 100000, 100000);

				//  Set radius of zero allows parent to animate even if outside of frustrum view
				if (GetNumberOfFrames(t.entobj) > 0)
				{
					//  but ONLY for animating objects, do not need to run parent objects if still
					SetSphereRadius(t.entobj, 0);
				}
			}
		}
		else
		{
			//  prevent crash when model name wrong/geometry file missing/etc
			MakeObjectSphere(t.entobj, 1);
			PositionObject(t.entobj, 100000, 100000, 100000);
		}

		//  must hide parent objects
		HideObject(t.entobj);

		//  Resolve default weapon gun ids
		if (t.entityprofile[t.entid].isweapon_s != "")
		{
			t.findgun_s = Lower(t.entityprofile[t.entid].isweapon_s.Get()); gun_findweaponindexbyname();
			t.entityprofile[t.entid].isweapon = t.foundgunid;
			if (t.foundgunid > 0)  t.gun[t.foundgunid].activeingame = 1;

			// all weapons are naturally collectable by default
			t.entityprofile[t.entid].iscollectable = 1;
		}
		else
		{
			t.entityprofile[t.entid].isweapon = 0;
		}

		//  Finding hasweapon also in createlemenents (as eleprof may have changed the weapon!)
		if (t.entityprofile[t.entid].hasweapon_s != "")
		{
			t.findgun_s = Lower(t.entityprofile[t.entid].hasweapon_s.Get()); gun_findweaponindexbyname();
			t.entityprofile[t.entid].hasweapon = t.foundgunid;
			if (t.foundgunid > 0 && t.entityprofile[t.entid].isammo == 0)
			{
				// make gun active in game
				t.gun[t.foundgunid].activeingame = 1;

				// 301115 - also populate profile with correct default ammo from clip if default required
				if (t.entityprofile[t.entid].ischaracter == 1)
				{
					if (t.entityprofile[t.entid].quantity == -1)
					{
						t.entityprofile[t.entid].quantity = t.gun[t.foundgunid].settings.reloadqty;
						if (t.entityprofile[t.entid].quantity == 0)
						{
							// discover reload quantity if gun data not loaded in
							t.gunid = t.foundgunid;
							t.gun_s = t.gun[t.gunid].name_s;
							gun_loaddata();
							t.entityprofile[t.entid].quantity = g.firemodes[t.gunid][0].settings.reloadqty;
							t.gunid = 0;
						}
					}
				}
			}
		}
		else
		{
			t.entityprofile[t.entid].hasweapon = 0;
		}

		// see if we can find head automatically
		// 010818 - expanded to find mixamo_xx or bip01_xx or anything_xx
		if (t.entityprofile[t.entid].ischaracter == 1)
		{
			if (t.entityprofile[t.entid].headlimb == -1)
			{
				if (ObjectExist(t.entobj) == 1)
				{
					PerformCheckListForLimbs(t.entobj);
					for (t.tc = 1; t.tc <= ChecklistQuantity(); t.tc++)
					{
						cstr sChecklistFound = Lower(ChecklistString(t.tc));
						LPSTR pChecklistFound = sChecklistFound.Get();
						if (strnicmp(pChecklistFound + strlen(pChecklistFound) - 5, "_head", 5) == NULL)
						{
							t.entityprofile[t.entid].headlimb = t.tc - 1;
							t.tc = ChecklistQuantity() + 1;
						}
					}
				}
			}
			if (t.entityprofile[t.entid].firespotlimb == -1)
			{
				if (ObjectExist(t.entobj) == 1)
				{
					// standard FIRESPOT search
					PerformCheckListForLimbs(t.entobj);
					int iRememberPositionOfRightHand = -1;
					int iLimbCount = ChecklistQuantity();
					for (t.tc = 1; t.tc <= iLimbCount; t.tc++)
					{
						cstr sLimbName = cstr(Lower(ChecklistString(t.tc)));
						if (sLimbName == "firespot")
						{
							t.entityprofile[t.entid].firespotlimb = t.tc - 1;
							t.tc = ChecklistQuantity() + 1;
						}
						else
						{
							if (strstr(sLimbName.Get(),"_r_hand")!=NULL)
							{
								iRememberPositionOfRightHand = t.tc - 1;
							}
						}
					}
					if (t.entityprofile[t.entid].firespotlimb == -1)
					{
						// could not find a FIRESPOT for this character, so create one if found right hand
						if (iRememberPositionOfRightHand != -1)
						{
							// for now, use right hand
							t.entityprofile[t.entid].firespotlimb = iRememberPositionOfRightHand;
						}
					}
				}
			}
			if (t.entityprofile[t.entid].spine == -1)
			{
				if (ObjectExist(t.entobj) == 1)
				{
					PerformCheckListForLimbs(t.entobj);
					for (t.tc = 1; t.tc <= ChecklistQuantity(); t.tc++)
					{
						cstr sChecklistFound = Lower(ChecklistString(t.tc));
						LPSTR pChecklistFound = sChecklistFound.Get();
						// LB: as it turns out, it is Bip01 that has the major say in how the model shuffles forward!
						if ( stricmp(pChecklistFound, "Bip1") == NULL
						||   stricmp(pChecklistFound, "Bip01") == NULL
						||   stricmp(pChecklistFound, "Bip001") == NULL)
						{
							t.entityprofile[t.entid].spine = t.tc - 1;
							break;
						}
					}
				}
			}
			if (t.entityprofile[t.entid].spine2 == -1)
			{
				if (ObjectExist(t.entobj) == 1)
				{
					PerformCheckListForLimbs(t.entobj);
					for (t.tc = 1; t.tc <= ChecklistQuantity(); t.tc++)
					{
						cstr sChecklistFound = Lower(ChecklistString(t.tc));
						LPSTR pChecklistFound = sChecklistFound.Get();
						if (strnicmp(pChecklistFound + strlen(pChecklistFound) - 7, "_spine2", 7) == NULL)
						{
							t.entityprofile[t.entid].spine2 = t.tc - 1;
							break;
						}
					}
				}
			}
		}

		// 090217 - new feature for some characters (Fuse FBX) to have perfect foot planting
		if (t.entityprofile[t.entid].ischaracter == 1 && t.entityprofile[t.entid].isspinetracker == 1 && t.entityprofile[t.entid].spine != -1)
		{
			sObject* pObject = GetObjectData(t.entobj);
			if (pObject)
			{
				pObject->bUseSpineCenterSystem = true;
				pObject->dwSpineCenterLimbIndex = t.entityprofile[t.entid].spine;
				pObject->fSpineCenterTravelDeltaX = 0.0f;
				pObject->fSpineCenterTravelDeltaZ = 0.0f;
			}
		}
		else
		{
			// 051115 - if character, ensure the Y offset is applied to the parent object
			if (t.entityprofile[t.entid].ischaracter == 1 || t.entityprofile[t.entid].offyoverride != 0)
			{
				// calc bounds for static entities that have hierarchy that need shifting up when offyoverride used
				int iCalculateBounds = 0;
				if (t.entityprofile[t.entid].offyoverride != 0) iCalculateBounds = 1;
				OffsetLimb(t.entobj, 0, 0.0f, t.entityprofile[t.entid].offy, 0.0f, iCalculateBounds);
			}
		}

		// 300819 - reenable offset X and Z (for when geometry is shifted from center)
		if (t.entityprofile[t.entid].defaultstatic == 1 && t.entityprofile[t.entid].isimmobile == 1)
		{
			OffsetLimb(t.entobj, 0, t.entityprofile[t.entid].offx, t.entityprofile[t.entid].offy, t.entityprofile[t.entid].offz, 1);
		}

		// 010917 - hide any firespot limb meshes
		if (t.entityprofile[t.entid].firespotlimb > 0)
		{
			ExcludeLimbOn(t.entobj, t.entityprofile[t.entid].firespotlimb);
		}

		//  190115 - wipe out limb control on legacy models, not configured for rotation!
		if (t.entityprofile[t.entid].skipfvfconvert == 1)
		{
			//t.entityprofile[t.entid].firespotlimb=0; but can allow firespot to be retained
			t.entityprofile[t.entid].headlimb = 0;
			t.entityprofile[t.entid].spine = 0;
			t.entityprofile[t.entid].spine2 = 0;
		}

		if (t.entityprofile[t.entid].ismarker == 2)
		{
			// Ensure all lights start as points when first loaded as parent
			entity_updatelightobjtype(t.entobj, 0);
		}

		//PE: Additional settings.
		// For Wicked, cull mode controlled per-mesh with parent default as normal 
		//PE: Prefer WEMaterial over old cullmode
		bool bUseWEMaterial = false;
		if (t.entityprofile[t.entid].WEMaterial.MaterialActive)
		{
			WickedSetEntityId(t.entid);
			WickedSetElementId(0);
			sObject* pObject = g_ObjectList[t.entobj];
			if (pObject)
			{
				bUseWEMaterial = true;
				for (int iMeshIndex = 0; iMeshIndex < pObject->iMeshCount; iMeshIndex++)
				{
					sMesh* pMesh = pObject->ppMeshList[iMeshIndex];
					if (pMesh)
					{
						// set properties of mesh
						WickedSetMeshNumber(iMeshIndex);
						bool bDoubleSided = WickedDoubleSided();
						if (bDoubleSided)
						{
							pMesh->bCull = false;
							pMesh->iCullMode = 0;
							WickedCall_SetMeshCullmode(pMesh);
						}
						else
						{
							pMesh->iCullMode = 1;
							pMesh->bCull = true;
							WickedCall_SetMeshCullmode(pMesh);
						}
					}
				}
			}
			WickedSetEntityId(-1);
		}

		if (!bUseWEMaterial)
			SetObjectCull(t.entobj, 1);
		SetObjectLight(t.entobj, 0);
		if (t.entityprofile[t.entid].bIsDecal)
		{
			void SetupDecalObject(int obj, int elementID);
			SetupDecalObject(t.entobj,0);
		}
		SetObjectCollisionOff(t.entobj);

		WickedSetEntityId(t.entid);
		SetAlphaMappingOn(t.entobj, 100);

		SetObjectTransparency(t.entobj, t.entityprofile[t.entid].transparency);
		WickedSetEntityId(-1);

		//  debug info and timestamp list (if logging)
		if (  t.entobj>0 ) 
		{
			if (  ObjectExist(t.entobj) == 1 ) 
			{
				t.strwork = "" ; t.strwork = t.strwork + "Loaded "+Str(t.entid)+":"+t.ent_s + " (" + cstr(GetObjectPolygonCount(t.entobj)) + ")";
				timestampactivity(0, t.strwork.Get() );
			}
		}
	}
	else
	{

		//  debug info and timestamp list (if logging)
		t.strwork = ""; t.strwork = t.strwork + "Skipped "+Str(t.entid)+":"+t.ent_s;
		timestampactivity(0, t.strwork.Get() );
	}

	//  Decactivate auto generation of mipmaps when entity load finished
	SetImageAutoMipMap (  0 );

	// normal entity loaded
	return true;
}

int constexpr conststrlen( const char* str )
{
	return *str ? 1 + conststrlen(str + 1) : 0;
}

#define cmpStrConst( str, cmpVal ) \
{ \
	matched = true; \
	if ( strcmp(str, cmpVal) != 0 ) matched = false; \
}

// max length 16
#define cmpNStrConst( str, cmpVal ) \
{ \
	matched = true; \
	int constexpr len = conststrlen( cmpVal ); \
	if ( strncmp(str, cmpVal, len) != 0 ) matched = false; \
}

void entity_loaddata ( void )
{
	//  Protectf BIN file if no FPE backup (standalone run)
	t.tprotectBINfile=0;
	if (t.ent_s.Get()[1] == ':') t.entdir_s = "";


	t.tFPEName_s=t.entdir_s+t.ent_s;
	if (  FileExist(t.tFPEName_s.Get()) == 0 ) 
	{
		t.tprotectBINfile=1;
	}

	//  Ensure entity profile still exists
	t.entityprofileheader[t.entid].desc_s="";
	t.tprofile_s=Left(t.tFPEName_s.Get(),Len(t.tFPEName_s.Get())-4); t.tprofile_s += ".bin";
	if (  t.tprotectBINfile == 0 ) 
	{
		//  020715 - to solve BIN issue once and for all, delete them when load entity
		//  and only preserve for final standalone export (actually NO performance difference!)
		if (  FileExist(t.tprofile_s.Get()) == 1  )  DeleteAFile (  t.tprofile_s.Get() );
	}

	t.strwork = t.entdir_s+t.ent_s;
	if (  FileExist(t.strwork.Get()) == 1 || FileExist(t.tprofile_s.Get()) == 1 ) 
	{

	//  Export entity FPE file if flagged
	if (  g.gexportassets == 1 ) 
	{
		t.strwork = t.entdir_s+t.ent_s;
		t.tthumbbmpfile_s = "";	t.tthumbbmpfile_s=t.tthumbbmpfile_s + Left(t.strwork.Get(),(Len(t.entdir_s.Get())+Len(t.ent_s.Get()))-4)+".bmp";
	}

	//  Allowed to loop around if skipBIN flag set
	do {  t.skipBINloadingandtryagain=0;

	//  Check if binary version of entity profile exists (DELETE BIN AT MOMEMENT!)
	//C++ISSUE forcing non binary at the moment due to make memblock from array not implemented for new arrays
	//looks like we are delelting bins anyway, but just incase...
	if ( 1 ) //if (  FileExist(t.tprofile_s.Get()) == 0 ) 
	{
		// 061115 - reset entity anim start value (so can be filled further down)
		for ( int n = 0; n < g.animmax; n++ )
		{
			t.entityanim[t.entid][n].start = 0;
			t.entityanim[t.entid][n].finish = 0;
			t.entityanim[t.entid][n].found = 0;
		}

		//  Must be reset before parse
		t.entityprofile[t.entid].limbmax=0;
		t.entityprofile[t.entid].animmax=0;
		t.entityprofile[t.entid].appendanimmax=0; //PE: sometimes , caused endless loop, was never set anywhere.
		t.entityprofile[t.entid].footfallmax=0;
		t.entityprofile[t.entid].headlimb=-1;
		t.entityprofile[t.entid].firespotlimb=-1;
		t.entityprofile[t.entid].physics=1;
		t.entityprofile[t.entid].phyweight=100;
		t.entityprofile[t.entid].phyfriction=0;
		t.entityprofile[t.entid].hoverfactor=0;
		t.entityprofile[t.entid].phyalways=0;
		t.entityprofile[t.entid].spine=-1;
		t.entityprofile[t.entid].spine2=-1;
		t.entityprofile[t.entid].decaloffsetangle=0;
		t.entityprofile[t.entid].decaloffsetdist=0;
		t.entityprofile[t.entid].decaloffsety=0;
		t.entityprofile[t.entid].ragdoll = 0;
		t.entityprofile[t.entid].nothrowscript=0;
		t.entityprofile[t.entid].canfight=1;
		t.entityprofile[t.entid].rateoffire=85;
		t.entityprofile[t.entid].transparency=0;
		t.entityprofile[t.entid].canseethrough=0;
		t.entityprofile[t.entid].cullmode=0;
		t.entityprofile[t.entid].lod1distance=0;
		t.entityprofile[t.entid].lod2distance=0;
		t.entityprofile[t.entid].characterbasetype = -1;// bitbobon = -1;
		t.entityprofile[t.entid].reservedy2 = 0.0f;
		t.entityprofile[t.entid].reservedy3 = 0.0f;
		t.entityprofile[t.entid].autoflatten=0;
		t.entityprofile[t.entid].headframestart=-1;
		t.entityprofile[t.entid].headframefinish=-1;
		t.entityprofile[t.entid].hairframestart=-1;
		t.entityprofile[t.entid].hairframefinish=-1;
		t.entityprofile[t.entid].hideframestart=-1;
		t.entityprofile[t.entid].hideframefinish=-1;
		t.entityprofile[t.entid].animspeed=100;
		t.entityprofile[t.entid].animstyle=0;
		t.entityprofile[t.entid].collisionscaling = 100;
		t.entityprofile[t.entid].collisionscalingxz = 100;
		t.entityprofile[t.entid].physicsobjectcount = 0;
		t.entityprofile[t.entid].ishudlayer_s="";
		t.entityprofile[t.entid].ishudlayer=0;
		t.entityprofile[t.entid].fatness=50;
		t.entityprofile[t.entid].matrixmode=0;
		t.entityprofile[t.entid].skipfvfconvert=0;
		t.entityprofile[t.entid].resetlimbmatrix=0;
		t.entityprofile[t.entid].onetexture=0;
		t.entityprofile[t.entid].usesweapstyleanims=0;
		t.entityprofile[t.entid].isviolent=1;
		t.entityprofile[t.entid].reverseframes=0;
		t.entityprofile[t.entid].fullbounds=0;
		t.entityprofile[t.entid].cpuanims=0;
		t.entityprofile[t.entid].ignoredefanim=0;
		t.entityprofile[t.entid].explodeheight=0;
		t.entityprofile[t.entid].explodable_decalname = "";
		t.entityprofile[t.entid].scale=100;
		t.entityprofile[t.entid].addhandle_s="";
		t.entityprofile[t.entid].addhandlelimb=0;
		t.entityprofile[t.entid].ischaractercreator=0;
		t.entityprofile[t.entid].charactercreator_s="";
		t.entityprofile[t.entid].fJumpModifier=1.0f;		
		t.entityprofile[t.entid].jumphold=0;
		t.entityprofile[t.entid].jumpresume=0;
		t.entityprofile[t.entid].jumpvaulttrim=1;
		t.entityprofile[t.entid].meleerange=80;
		t.entityprofile[t.entid].meleehitangle=30;
		t.entityprofile[t.entid].meleestrikest=0;
		t.entityprofile[t.entid].meleestrikefn=0;
		t.entityprofile[t.entid].meleedamagest=20;
		t.entityprofile[t.entid].meleedamagefn=30;
		for ( t.q = 0 ; t.q<=  100 ; t.q++ ) { t.entitybodypart[t.entid][t.q]=0 ;   }
		t.entityprofile[t.entid].usespotlighting=0;
		t.entityprofile[t.entid].lodmodifier=0;
		t.entityprofile[t.entid].isocluder=1; // can be adjusted (if notanoccluder set to 1)
		t.entityprofile[t.entid].isocludee=1;
		t.entityprofile[t.entid].lootpercentage=100;
		t.entityprofile[t.entid].specular=0;
		t.entityprofile[t.entid].uvscrollu=0;
		t.entityprofile[t.entid].uvscrollv=0;
		t.entityprofile[t.entid].uvscaleu=1.0f;
		t.entityprofile[t.entid].uvscalev=1.0f;
		t.entityprofile[t.entid].invertnormal=0;
		t.entityprofile[t.entid].preservetangents=0;		
		t.entityprofile[t.entid].colondeath=1;
		t.entityprofile[t.entid].parententityindex=0;
		t.entityprofile[t.entid].parentlimbindex=0;
		t.entityprofile[t.entid].quantity=-1; // FPE specifies a value or we use a single weapon clip buy default (below)
		t.entityprofile[t.entid].smoothangle=0;
		t.entityprofile[t.entid].noXZrotation=0;
		t.entityprofile[t.entid].zdepth = 1;
		t.entityprofile[t.entid].isebe = 0;
		t.entityprofile[t.entid].offyoverride = 0;
		t.entityprofile[t.entid].offy = 0; //PE: Was not reset.
		t.entityprofile[t.entid].ismarker = 0;  //PE: Was not reset.
		t.entityprofile[t.entid].ischaracter = 0;
		t.entityprofile[t.entid].bIsDecal = false; //PE: Was not set.
		t.entityprofile[t.entid].isspinetracker = 1;
		t.entityprofile[t.entid].phyweight=100;
		t.entityprofile[t.entid].phyfriction=100;
		t.entityprofile[t.entid].phyforcedamage=100;
		t.entityprofile[t.entid].rotatethrow=1;
		t.entityprofile[t.entid].explodedamage=100;
		t.entityprofile[t.entid].forcesimpleobstacle=0;
		t.entityprofile[t.entid].forceobstaclepolysize=20.0f;//30.0f; hagia model
		t.entityprofile[t.entid].forceobstaclesliceheight=20.0f;//14.0f; hagia model
		t.entityprofile[t.entid].forceobstaclesliceminsize=4.0f;//5.0f; hagia model 
		t.entityprofile[t.entid].effectprofile=0;
		t.entityprofile[t.entid].ignorecsirefs=0;
		t.entityprofile[t.entid].voiceset_s = ""; // when empty, default to first voice
		t.entityprofile[t.entid].voicerate = 0;
		// For MAX, these HANDFIRESPOT values may be purposed if different characters needed a hand adjust to fit weapon standard (not in EA)
		t.entityprofile[t.entid].handfirespotoffx = 0.0f;
		t.entityprofile[t.entid].handfirespotoffy = 0.0f;
		t.entityprofile[t.entid].handfirespotoffz = 0.0f;
		t.entityprofile[t.entid].handfirespotrotx = 0.0f;
		t.entityprofile[t.entid].handfirespotroty = 0.0f;
		t.entityprofile[t.entid].handfirespotrotz = 0.0f;
		t.entityprofile[t.entid].handfirespotsize = 100.0f;
		t.entityprofile[t.entid].coneangle = 100.0f;
		t.entityprofile[t.entid].conerange = 1000.0f;

		// reset so characters start with NO WEAPON!
		t.entityprofile[t.entid].isammo = 0;
		t.entityprofile[t.entid].hasweapon = 0;
		t.entityprofile[t.entid].hasweapon_s = "";

		// head and spine tracker detail defaults
		t.entityprofile[t.entid].headspinetracker.headhlimit = 60;// 45; CineGuru better talk tracking (can improve down the road with eye and gesture tracking)!
		t.entityprofile[t.entid].headspinetracker.headhoffset = 0;
		t.entityprofile[t.entid].headspinetracker.headvlimit = 45;
		t.entityprofile[t.entid].headspinetracker.headvoffset = 0;
		t.entityprofile[t.entid].headspinetracker.spinehlimit = 45;
		t.entityprofile[t.entid].headspinetracker.spinehoffset = 0;
		t.entityprofile[t.entid].headspinetracker.spinevlimit = 45;
		t.entityprofile[t.entid].headspinetracker.spinevoffset = 0;

		//  Starter animation counts
		t.tnewanimmax=0 ; t.entityprofile[t.entid].animmax=t.tnewanimmax;
		t.tstartofaianim=-1 ; t.entityprofile[t.entid].startofaianim=t.tstartofaianim;

		// other resets
		t.entityappendanim[t.entid][0].filename = "";
		t.entityappendanim[t.entid][0].startframe = 0;

		// wicked custom materials
		t.entityprofile[t.entid].thumbnailbackdrop = "";
		t.entityprofile[t.entid].WEMaterial.MaterialActive = false;

		for (int i = 0; i < MAXMESHMATERIALS; i++)
		{
			// per mesh textures and control values

			t.entityprofile[t.entid].WEMaterial.customShaderID = -1;
			t.entityprofile[t.entid].WEMaterial.customShaderParam1 = 1;
			t.entityprofile[t.entid].WEMaterial.customShaderParam2 = 1;
			t.entityprofile[t.entid].WEMaterial.customShaderParam3 = 1;
			t.entityprofile[t.entid].WEMaterial.customShaderParam4 = 1;
			t.entityprofile[t.entid].WEMaterial.customShaderParam5 = 1;
			t.entityprofile[t.entid].WEMaterial.customShaderParam6 = 1;
			t.entityprofile[t.entid].WEMaterial.customShaderParam7 = 1;
			t.entityprofile[t.entid].WEMaterial.WPEffect = "";

			t.entityprofile[t.entid].WEMaterial.baseColorMapName[i] = "";
			t.entityprofile[t.entid].WEMaterial.normalMapName[i] = "";
			t.entityprofile[t.entid].WEMaterial.emissiveMapName[i] = "";
			t.entityprofile[t.entid].WEMaterial.surfaceMapName[i] = "";
			#ifndef DISABLEOCCLUSIONMAP
			t.entityprofile[t.entid].WEMaterial.occlusionMapName[i] = "";
			#endif
			t.entityprofile[t.entid].WEMaterial.displacementMapName[i] = "";
			t.entityprofile[t.entid].WEMaterial.fAlphaRef[i] = 1.0;
			t.entityprofile[t.entid].WEMaterial.fNormal[i] = 1.0;
			t.entityprofile[t.entid].WEMaterial.fEmissive[i] = 0.0;
			t.entityprofile[t.entid].WEMaterial.fRoughness[i] = 1.0;
			t.entityprofile[t.entid].WEMaterial.fMetallness[i] = 1.0;

			// per mesh colors
			t.entityprofile[t.entid].WEMaterial.dwBaseColor[i] = -1;

			//PE: Default to black, if old .fpe files do not have it set.
			t.entityprofile[t.entid].WEMaterial.dwEmmisiveColor[i] = 0;

			// per mesh settings and flags
			t.entityprofile[t.entid].WEMaterial.bCastShadows[i] = true;
			t.entityprofile[t.entid].WEMaterial.bDoubleSided[i] = false;
			t.entityprofile[t.entid].WEMaterial.fRenderOrderBias[i] = 0;
			t.entityprofile[t.entid].WEMaterial.bPlanerReflection[i] = false;
			t.entityprofile[t.entid].WEMaterial.bTransparency[i] = false;
			t.entityprofile[t.entid].WEMaterial.fReflectance[i] = 0.04f;// 0.002f;
		}

		t.entityprofile[t.entid].thumbnailbackdrop = "";
		t.entityprofile[t.entid].BackBufferZoom = -1.0f;
		t.entityprofile[t.entid].BackBufferCamLeft = -1.0f;
		t.entityprofile[t.entid].BackBufferCamUp = -1.0f;
		t.entityprofile[t.entid].BackBufferRotateX = -1.0f;
		t.entityprofile[t.entid].BackBufferRotateY = -1.0f;
		t.entityprofile[t.entid].iThumbnailAnimset = -1.0f;
		t.entityprofile[t.entid].keywords_s = "";
		
		t.entityprofile[t.entid].effect_s = ""; //bIsDecal , needed reset.

		t.entityprofile[t.entid].collectable.image = "default";
		t.entityprofile[t.entid].collectable.description = t.entityprofileheader[t.entid].desc_s;
		t.entityprofile[t.entid].collectable.cost = 10;
		t.entityprofile[t.entid].collectable.value = 5;
		t.entityprofile[t.entid].collectable.container = "none";
		t.entityprofile[t.entid].collectable.ingredients = "none";
		t.entityprofile[t.entid].collectable.style = "none";

		t.entityprofile[t.entid].blendmode = 0;

		t.entityprofile[t.entid].light.fProbeBrightness = 1.0f;

		//  temp variable to hold which physics object we are on from the importer
		t.tPhysObjCount = 0;

		//  Load entity Data from file
		// ZJ: Users have been reporting their imported models don't load correctly, caused by the FPEs having more than 400 lines.
		Dim(t.data_s, 500); //Dim (  t.data_s,400  );
		t.strwork = t.entdir_s + t.ent_s;
		LoadArray(t.strwork.Get(), t.data_s);
		for (t.l = 0; t.l <= 499; t.l++) //for ( t.l = 0 ; t.l<=  399; t.l++ )*/
		{
			t.line_s=t.data_s[t.l];
			if (  Len(t.line_s.Get())>0 ) 
			{
				if (  t.line_s.Get()[0] != ';' ) 
				{
					// safety check to ensure strings not overloaded (i.e. if trying to load an encrypted carbage FPE)
					if (strlen(t.line_s.Get()) > 255)
					{
						t.value_s = "ERROR";
						continue;
					}

					//  take fieldname and value
					for ( t.c = 0 ; t.c < Len(t.line_s.Get()); t.c++ )
					{
						if ( t.line_s.Get()[t.c] == '=' ) 
						{ 
							t.mid = t.c+1  ; break;
						}
					}
					t.field_s=cstr(Lower(removeedgespaces(Left(t.line_s.Get(),t.mid-1))));
					t.value_s=cstr(removeedgespaces(Right(t.line_s.Get(),Len(t.line_s.Get())-t.mid)));

					//  take value 1 and 2 from value
					for ( t.c = 0 ; t.c < Len(t.value_s.Get()); t.c++ )
					{
						if (  t.value_s.Get()[t.c] == ',' ) 
						{ 
							t.mid = t.c+1 ; break; 
						}
					}
					t.value1=ValF(removeedgespaces(Left(t.value_s.Get(),t.mid-1)));
					t.value1_f=ValF(removeedgespaces(Left(t.value_s.Get(),t.mid-1)));
					t.value2_s=cstr(removeedgespaces(Right(t.value_s.Get(),Len(t.value_s.Get())-t.mid)));
					if (  Len(t.value2_s.Get())>0  )  t.value2 = ValF(t.value2_s.Get()); else t.value2 = -1;

					// string comparison optimization
					alignas(16) char t_field_s[32];
					const char* src = Lower(t.field_s.Get());
					int index = 0;
					while( src[index] && index < 32 )
					{
						t_field_s[ index ] = src[ index ];
						index++;
					}

					int matchlen = index;

					while( index < 32 )
					{
						t_field_s[ index++ ] = 0;
					}

					bool matched = false;

					//  entity header
					cmpStrConst( t_field_s, "desc" );
					if ( matched )  t.entityprofileheader[t.entid].desc_s = t.value_s;
					
					// check if group early, and abort (we use LoadGroup for these type of FPE files)
					cmpStrConst( t_field_s, "isgroupobject" );
					if (matched && t.value1 == 1)
					{
						// if looking for the group flag, mark it as found
						extern int g_iAbortedAsEntityIsGroupFileMode;
						if (g_iAbortedAsEntityIsGroupFileMode == 1)
						{
							// move mode to show we have determined it IS a group file!
							g_iAbortedAsEntityIsGroupFileMode = 2;
						}
					}

					//  entity AI
					cmpStrConst( t_field_s, "aimain" );
					if (matched)
					{
						t.entityprofile[t.entid].aimain_s = Lower(t.value_s.Get());
						if (strnicmp (t.entityprofile[t.entid].aimain_s.Get(), "default.lua", 11) == NULL)
						{
							t.entityprofile[t.entid].aimain_s = "no_behavior_selected.lua";
						}
						else
						{
							if (strlen(t.entityprofile[t.entid].aimain_s.Get()) < 4 ) 
							{
								t.entityprofile[t.entid].aimain_s = "no_behavior_selected.lua";
							}
						}
						if (strnicmp (t.entityprofile[t.entid].aimain_s.Get(), "markers\\togglelight.lua", 23) == NULL)
						{
							// ensure this particular behavior uses case (eventually allow all objects to use case (carefully))
							// doing so right now might introduce new issues!
							t.entityprofile[t.entid].aimain_s = "markers\\ToggleLight.lua";
						}
					}

					// determine CCP base type and store
					cmpStrConst( t_field_s, "ccpassembly" );
					if (matched)
					{
						char pLowCase[MAX_PATH];
						strcpy(pLowCase, t.value_s.Get());
						strlwr(pLowCase);
						t.entityprofile[t.entid].characterbasetype = -1;
						if (strstr (pLowCase, "adult male") != NULL) t.entityprofile[t.entid].characterbasetype = 0;
						if (strstr (pLowCase, "adult female") != NULL) t.entityprofile[t.entid].characterbasetype = 1;
						if (strstr (pLowCase, "zombie male") != NULL) t.entityprofile[t.entid].characterbasetype = 2;
						if (strstr (pLowCase, "zombie female") != NULL) t.entityprofile[t.entid].characterbasetype = 3;
						if (t.entityprofile[t.entid].characterbasetype == -1)
						{
							for (int i = 0; i < g_CharacterType.size(); i++)
							{
								if (strnicmp (pLowCase, g_CharacterType[i].pPartsFolder, strlen(g_CharacterType[i].pPartsFolder)) == NULL)
								{
									t.entityprofile[t.entid].characterbasetype = i;
									break;
								}
							}
						}
					}

					cmpStrConst( t_field_s, "voice" );
					if (  matched  )  t.entityprofile[t.entid].voiceset_s = t.value_s;
					cmpStrConst( t_field_s, "speakrate" );
					if (  matched  )  t.entityprofile[t.entid].voicerate = t.value1;
					cmpStrConst( t_field_s, "soundset" );
					if (  matched  )  t.entityprofile[t.entid].soundset_s = t.value_s;
					cmpStrConst( t_field_s, "soundset1" );
					if (  matched  )  t.entityprofile[t.entid].soundset1_s = t.value_s;
					cmpStrConst( t_field_s, "soundset2" );
					if (  matched  )  t.entityprofile[t.entid].soundset2_s = t.value_s;
					cmpStrConst( t_field_s, "soundset3" );
					if (  matched  )  t.entityprofile[t.entid].soundset3_s = t.value_s;
					cmpStrConst( t_field_s, "soundset4");
					if (  matched  )  t.entityprofile[t.entid].soundset4a_s = t.value_s;
					cmpStrConst( t_field_s, "soundset5");
					if (  matched  )  t.entityprofile[t.entid].soundset5_s = t.value_s;
					cmpStrConst( t_field_s, "soundset6");
					if (  matched  )  t.entityprofile[t.entid].soundset6_s = t.value_s;
					//  entity AI related vars
					cmpStrConst( t_field_s, "usekey" );
					if (  matched  )  t.entityprofile[t.entid].usekey_s = t.value_s;
					cmpStrConst( t_field_s, "ifused" );
					if (  matched  )  t.entityprofile[t.entid].ifused_s = t.value_s;

					//  entity SPAWN
					cmpStrConst( t_field_s, "spawnmax" );
					if (  matched  )  t.entityprofile[t.entid].spawnmax = t.value1;
					cmpStrConst( t_field_s, "spawndelay" );
					if (  matched  )  t.entityprofile[t.entid].spawndelay = t.value1;
					cmpStrConst( t_field_s, "spawnqty" );
					if (  matched  )  t.entityprofile[t.entid].spawnqty = t.value1;
					//  entity orientation
					cmpStrConst( t_field_s, "model" );
					//  if it is a character creator model, ignore this
					if (  t.entityprofile[t.entid].ischaractercreator == 0 ) 
					{
						if (  matched  ) 
							t.entityprofile[t.entid].model_s = t.value_s;
					}

					cmpStrConst( t_field_s, "offsetx" );
					if (  matched  )  t.entityprofile[t.entid].offx = t.value1;
					cmpStrConst( t_field_s, "offsety" );
					if (  matched  )  t.entityprofile[t.entid].offy = t.value1;
					cmpStrConst( t_field_s, "offyoverride" );
					if (  matched  )  
					{
						t.entityprofile[t.entid].offyoverride = 1;
						t.entityprofile[t.entid].offy = t.value1;
					}
					cmpStrConst( t_field_s, "offsetz" );
					if (  matched  )  t.entityprofile[t.entid].offz = t.value1;
					cmpStrConst( t_field_s, "rotx" );
					if (  matched  )  t.entityprofile[t.entid].rotx = t.value1;
					cmpStrConst( t_field_s, "roty" );
					if (  matched  )  t.entityprofile[t.entid].roty = t.value1;
					cmpStrConst( t_field_s, "rotz" );
					if (  matched  )  t.entityprofile[t.entid].rotz = t.value1;
					cmpStrConst( t_field_s, "scale" );
					if (  matched  )  t.entityprofile[t.entid].scale = t.value1;
					cmpStrConst( t_field_s, "fixnewy" );
					if (  matched  )  t.entityprofile[t.entid].fixnewy = t.value1;
					cmpStrConst( t_field_s, "hoverfactor" );
					if (  matched ) 
					{
						//  FPGC - V116 - some FPE characters use a range 0.1-0.9, must be accounted!
						if (  t.value1_f>-1.0 && t.value1_f<1.0  )  t.value1 = 1;
						t.entityprofile[t.entid].hoverfactor=t.value1;
						t.entityprofile[t.entid].hoverfactor=t.value1;
					}
					cmpStrConst( t_field_s, "forwardfacing" );
					if (  matched  )  t.entityprofile[t.entid].forwardfacing = t.value1;
					cmpStrConst( t_field_s, "dontfindfloor" );
					if (  matched  )  t.entityprofile[t.entid].dontfindfloor = t.value1;
					cmpStrConst( t_field_s, "defaultheight" );
					if (  matched  )  t.entityprofile[t.entid].defaultheight = t.value1;
					cmpStrConst( t_field_s, "defaultstatic" );
					if (  matched  )  t.entityprofile[t.entid].defaultstatic = t.value1;

					//  autoflatten
					//  0  ; none (default)
					//  1  ; rectangle area
					//  2  ; circle area
					cmpStrConst( t_field_s, "autoflatten" );
					if (  matched  )  t.entityprofile[t.entid].autoflatten = t.value1;

					//  collisionmode (see GameGuru\Docs\collisionmodevalues.txt)
					//  0  ; box shape (default)
					//  1  ; polygon shape
					//  2  ; sphere shape
					//  3  ; cylinder shape
					//  8  ; polygon shape using OBJ file
					//  9  ; convex hull reduction shape
					//  10 ; hull decomposition - multiple convex hulls
					//  11 ; no physics
					//  12 ; no physics but can still be detected with IntersectAll command
					//  21 ; player repel feature (for characters and other beasts/zombies)
					//  22 ; no repel (for animals that player can pass through)
					//  31 ; hybrid collision (dynamic box shape except meshes ending with "_static" which are separated as non colliding static renders; full doors/windows set) 
					//  40 ; collision boxes (defined in Import Model feature)
					//  41-49 ; reserved (collision polylist, sphere list, cylinder list)
					//  50 ; generate obstacle and cylinder from 1/64th up from base of model
					//  51 ; generate obstacle and cylinder from 1/32th down from base of model
					//  52 ; generate obstacle and cylinder from 8/16th up from base of model
					//  53 ; generate obstacle and cylinder from 7/16th up from base of model
					//  54 ; generate obstacle and cylinder from 6/16th up from base of model
					//  55 ; generate obstacle and cylinder from 5/16th up from base of model
					//  56 ; generate obstacle and cylinder from 4/16th up from base of model
					//  57 ; generate obstacle and cylinder from 3/16th up from base of model
					//  58 ; generate obstacle and cylinder from 2/16th up from base of model
					//  59 ; generate obstacle and cylinder from 1/16th up from base of model
					//  1000-2000 ; only one limb has collision Box Shape (1000=limb zero,1001=limb one,etc)
					//  2000-3000 ; only one limb has collision Polygons Shape (2000=limb zero,2001=limb one,etc)					
					cmpStrConst( t_field_s, "collisionmode" );
					if (  matched  )  t.entityprofile[t.entid].collisionmode = t.value1;
					cmpStrConst( t_field_s, "collisionscaling" );
					if (matched)
					{
						t.entityprofile[t.entid].collisionscaling = t.value1;
						if(t.value2>0) t.entityprofile[t.entid].collisionscalingxz = t.value2;
					}
					cmpStrConst( t_field_s, "collisionoverride" );
					if (  matched  )  t.entityprofile[t.entid].collisionoverride = t.value1;

					// endcollision:
					// 0 - no collision for ragdoll
					// 1 - collision for ragdoll
					cmpStrConst( t_field_s, "endcollision" );
					if (  matched  )  t.entityprofile[t.entid].colondeath = t.value1;

					//  forcesimpleobstacle
					//  -1 ; absolutely no obstacle
					//  0 ; default
					//  1 ; Box (  )
					//  2 ; contour
					//  3 ; full poly scan
					cmpStrConst( t_field_s, "forcesimpleobstacle" );
					if (  matched  )  t.entityprofile[t.entid].forcesimpleobstacle = t.value1;
					cmpStrConst( t_field_s, "forceobstaclepolysize" );
					if (  matched  )  t.entityprofile[t.entid].forceobstaclepolysize = t.value1;
					cmpStrConst( t_field_s, "forceobstaclesliceheight" );
					if (  matched  )  t.entityprofile[t.entid].forceobstaclesliceheight = t.value1;
					cmpStrConst( t_field_s, "forceobstaclesliceminsize" );
					if (  matched  )  t.entityprofile[t.entid].forceobstaclesliceminsize = t.value1;

					cmpStrConst( t_field_s, "notanoccluder" );
					if (  matched  )  
					{
						t.entityprofile[t.entid].notanoccluder = t.value1;
						if ( t.entityprofile[t.entid].notanoccluder == 1 ) t.entityprofile[t.entid].isocluder = 0;
					}
					cmpStrConst( t_field_s, "notanoccludee" );
					if (  matched  )  
					{
						int notanoccludee = t.value1;
						if ( notanoccludee == 1 ) t.entityprofile[t.entid].isocludee = 0;
					}

					cmpStrConst( t_field_s, "skipfvfconvert" );
					if (  matched  )  t.entityprofile[t.entid].skipfvfconvert = t.value1;
					cmpStrConst( t_field_s, "matrixmode" );
					if (  matched  )  t.entityprofile[t.entid].matrixmode = t.value1;

					// 040116 - when lightmapper scales entities incorrectly, need this flag to correct!
					cmpStrConst( t_field_s, "resetlimbmatrix" );
					if (  matched  )  t.entityprofile[t.entid].resetlimbmatrix = t.value1;

					cmpStrConst( t_field_s, "reverseframes" );
					if (  matched  )  t.entityprofile[t.entid].reverseframes = t.value1;
					cmpStrConst( t_field_s, "fullbounds" );
					if (  matched  )  t.entityprofile[t.entid].fullbounds = t.value1;

					//  cpuanims
					//  0 ; GPU animation
					//  1 ; CPU animation (for wide scope animations that need accurate ray detection)
					//  2 ; Same as [1] but will hide any meshes that do not have animations
					cmpStrConst( t_field_s, "cpuanims" );
					if (  matched  )  t.entityprofile[t.entid].cpuanims = t.value1;
				
					// 131115 - some legacy models hold an OLD nasty frame in matCombined for each frame, and can mess up collision detection if CPUANIMS=1 also
					cmpStrConst( t_field_s, "ignoredefanim" );
					if (  matched  )  t.entityprofile[t.entid].ignoredefanim = t.value1;

					//  materialindex
					// -1    = Can happen when Model Importer batch converts before global material manually set (bug-so repair below)!
					//  0    = - GenericSoft
					//  1    = S Stone
					//  2    = M Metal
					//  3    = W Wood
					//  4    = G Glass
					//  5    = L Liquid Splashy Wet
					//  6    = F Flesh (Bloody Organic)
					//  7    = H Hollow Drum Metal
					//  8    = T Small High Pitch Tin
					//  9    = V Small Low Pitch Tin
					//  10   = I Silly Material
					//  11   = A Marble
					//  12   = C Cobble
					//  13   = R Gravel
					//  14   = E Soft Metal
					//  15   = O Old Stone
					//  16   = D Old Wood
					//  17   = W Shallow Water
					//  18   = U Underwater
					cmpStrConst( t_field_s, "materialindex" );
					if (matched)
					{
						t.entityprofile[t.entid].materialindex = t.value1;
						if (t.entityprofile[t.entid].materialindex == -1) t.entityprofile[t.entid].materialindex = 0; // repair!
					}

					//  LOD and BITBOB system
					cmpStrConst( t_field_s, "disablebatch" );
					if (  matched  )  t.entityprofile[t.entid].disablebatch = t.value1;
					cmpStrConst( t_field_s, "lod1distance" );
					if (  matched  )  t.entityprofile[t.entid].lod1distance = t.value1;
					cmpStrConst( t_field_s, "lod2distance" );
					if (  matched  )  t.entityprofile[t.entid].lod2distance = t.value1;

					//  physics setup
					cmpStrConst( t_field_s, "physics" );
					if (  matched  )  t.entityprofile[t.entid].physics = t.value1;
					cmpStrConst( t_field_s, "phyweight" );
					if (  matched  )  t.entityprofile[t.entid].phyweight = t.value1;
					cmpStrConst( t_field_s, "phyfriction" );
					if (  matched  )  t.entityprofile[t.entid].phyfriction = t.value1;
					cmpStrConst( t_field_s, "explodable" );
					if (  matched  )  t.entityprofile[t.entid].explodable = t.value1;
					cmpStrConst( t_field_s, "explodedamage" );
					if (  matched  )  t.entityprofile[t.entid].explodedamage = t.value1;
					cmpStrConst( t_field_s, "ragdoll" );
					if (matched)
					{
						t.entityprofile[t.entid].ragdoll = t.value1;
					}

					//  FPGC - 160511 - added NOTHROWSCRIPT to entity profile
					cmpStrConst( t_field_s, "nothrowscript" );
					if (  matched  )  t.entityprofile[t.entid].nothrowscript = t.value1;

					//  cone of sight
					cmpStrConst( t_field_s, "coneheight" );
					if (  matched  )  t.entityprofile[t.entid].coneheight = t.value1;
					cmpStrConst( t_field_s, "coneangle" );
					if (  matched  )  t.entityprofile[t.entid].coneangle = t.value1;
					cmpStrConst( t_field_s, "conerange" );
					if (  matched  )  t.entityprofile[t.entid].conerange = t.value1;

					//  visual info
					cmpStrConst( t_field_s, "onetexture" );
					if (  matched  )  t.entityprofile[t.entid].onetexture = t.value1;
					if (t.entityprofile[t.entid].ischaractercreator == 0)
					{
						cmpStrConst( t_field_s, "texturepath" );
						if (matched)  t.entityprofile[t.entid].texpath_s = t.value_s;
						cmpStrConst( t_field_s, "textured" );
						if (matched)  t.entityprofile[t.entid].texd_s = t.value_s;
						cmpStrConst( t_field_s, "texturealtd" );
						if (matched)  t.entityprofile[t.entid].texaltd_s = t.value_s;


						// store thumbnail backdrop image name
						cmpStrConst( t_field_s, "thumbnailbackdrop" ); if (matched) { t.entityprofile[t.entid].thumbnailbackdrop = t.value_s; }
						cmpStrConst( t_field_s, "thumbnailzoom" ); if (matched) { t.entityprofile[t.entid].BackBufferZoom = t.value1_f; }
						cmpStrConst( t_field_s, "thumbnailcamleft" ); if (matched) { t.entityprofile[t.entid].BackBufferCamLeft = t.value1_f; }
						cmpStrConst( t_field_s, "thumbnailcamup" ); if (matched) { t.entityprofile[t.entid].BackBufferCamUp = t.value1_f; }
						cmpStrConst( t_field_s, "thumbnailrotatex" ); if (matched) { t.entityprofile[t.entid].BackBufferRotateX = t.value1_f; }
						cmpStrConst( t_field_s, "thumbnailrotatey" ); if (matched) { t.entityprofile[t.entid].BackBufferRotateY = t.value1_f; }
						cmpStrConst( t_field_s, "thumbnailanimset" ); if (matched) { t.entityprofile[t.entid].iThumbnailAnimset = (int) t.value1_f; }
						cmpStrConst( t_field_s, "keywords" ); if (matched) { t.entityprofile[t.entid].keywords_s = t.value_s; }

						cmpNStrConst( t_field_s, "basecolormap" );
						if ( matched )
						{
							int index = atoi( t_field_s + 12 );
							if ( index < MAXMESHMATERIALS )
							{
								t.entityprofile[t.entid].WEMaterial.baseColorMapName[index] = t.value_s; 
								t.entityprofile[t.entid].WEMaterial.MaterialActive = true;
							}
						}

						cmpNStrConst(t_field_s, "customshaderid");
						if (matched)
						{
							t.entityprofile[t.entid].WEMaterial.customShaderID = t.value1_f;
							t.entityprofile[t.entid].WEMaterial.MaterialActive = true;
						}
						cmpNStrConst(t_field_s, "customshaderparam1");
						if (matched)
						{
							t.entityprofile[t.entid].WEMaterial.customShaderParam1 = t.value1_f;
							t.entityprofile[t.entid].WEMaterial.MaterialActive = true;
						}
						cmpNStrConst(t_field_s, "customshaderparam2");
						if (matched)
						{
							t.entityprofile[t.entid].WEMaterial.customShaderParam2 = t.value1_f;
							t.entityprofile[t.entid].WEMaterial.MaterialActive = true;
						}
						cmpNStrConst(t_field_s, "customshaderparam3");
						if (matched)
						{
							t.entityprofile[t.entid].WEMaterial.customShaderParam3 = t.value1_f;
							t.entityprofile[t.entid].WEMaterial.MaterialActive = true;
						}
						cmpNStrConst(t_field_s, "customshaderparam4");
						if (matched)
						{
							t.entityprofile[t.entid].WEMaterial.customShaderParam4 = t.value1_f;
							t.entityprofile[t.entid].WEMaterial.MaterialActive = true;
						}
						cmpNStrConst(t_field_s, "customshaderparam5");
						if (matched)
						{
							t.entityprofile[t.entid].WEMaterial.customShaderParam5 = t.value1_f;
							t.entityprofile[t.entid].WEMaterial.MaterialActive = true;
						}
						cmpNStrConst(t_field_s, "customshaderparam6");
						if (matched)
						{
							t.entityprofile[t.entid].WEMaterial.customShaderParam6 = t.value1_f;
							t.entityprofile[t.entid].WEMaterial.MaterialActive = true;
						}
						cmpNStrConst(t_field_s, "customshaderparam7");
						if (matched)
						{
							t.entityprofile[t.entid].WEMaterial.customShaderParam7 = t.value1_f;
							t.entityprofile[t.entid].WEMaterial.MaterialActive = true;
						}


						cmpNStrConst( t_field_s, "alpharef" );
						if ( matched )
						{
							int index = atoi( t_field_s + 8 );
							if ( index < MAXMESHMATERIALS )
							{
								t.entityprofile[t.entid].WEMaterial.fAlphaRef[index] = t.value1_f; 
								t.entityprofile[t.entid].WEMaterial.MaterialActive = true;
							}
						}

						cmpNStrConst( t_field_s, "normalmap" );
						if ( matched )
						{
							int index = atoi( t_field_s + 9 );
							if ( index < MAXMESHMATERIALS )
							{
								t.entityprofile[t.entid].WEMaterial.normalMapName[index] = t.value_s; 
								t.entityprofile[t.entid].WEMaterial.MaterialActive = true;
							}
						}

						cmpNStrConst( t_field_s, "normalstrength" );
						if ( matched )
						{
							int index = atoi( t_field_s + 14 );
							if ( index < MAXMESHMATERIALS )
							{
								t.entityprofile[t.entid].WEMaterial.fNormal[index] = t.value1_f; 
								t.entityprofile[t.entid].WEMaterial.MaterialActive = true;
							}
						}

						cmpNStrConst( t_field_s, "surfacemap" );
						if ( matched )
						{
							int index = atoi( t_field_s + 10 );
							if ( index < MAXMESHMATERIALS )
							{
								t.entityprofile[t.entid].WEMaterial.surfaceMapName[index] = t.value_s;
								t.entityprofile[t.entid].WEMaterial.MaterialActive = true;
							}
						}

						cmpNStrConst( t_field_s, "roughnessstreng" );
						if ( matched )
						{
							int index = atoi( t_field_s + 17 );
							if ( index < MAXMESHMATERIALS )
							{
								t.entityprofile[t.entid].WEMaterial.fRoughness[index] = t.value1_f;
								t.entityprofile[t.entid].WEMaterial.MaterialActive = true;
							}
						}

						cmpNStrConst( t_field_s, "metalnessstreng" );
						if ( matched )
						{
							int index = atoi( t_field_s + 17 );
							if ( index < MAXMESHMATERIALS )
							{
								t.entityprofile[t.entid].WEMaterial.fMetallness[index] = t.value1_f;
								t.entityprofile[t.entid].WEMaterial.MaterialActive = true;
							}
						}

						cmpNStrConst( t_field_s, "displacementmap" );
						if ( matched )
						{
							int index = atoi( t_field_s + 15 );
							if ( index < MAXMESHMATERIALS )
							{
								t.entityprofile[t.entid].WEMaterial.displacementMapName[index] = t.value_s;
								t.entityprofile[t.entid].WEMaterial.MaterialActive = true;
							}
						}

						cmpNStrConst( t_field_s, "emissivemap" );
						if ( matched )
						{
							int index = atoi( t_field_s + 11 );
							if ( index < MAXMESHMATERIALS )
							{
								t.entityprofile[t.entid].WEMaterial.emissiveMapName[index] = t.value_s;
								t.entityprofile[t.entid].WEMaterial.MaterialActive = true;
							}
						}

						cmpNStrConst( t_field_s, "emissivestrengt" ); //PE: "emissivestrength"
						if ( matched )
						{
							int index = atoi( t_field_s + 16 );
							if ( index < MAXMESHMATERIALS )
							{
								t.entityprofile[t.entid].WEMaterial.fEmissive[index] = t.value1_f;
								t.entityprofile[t.entid].WEMaterial.MaterialActive = true;
							}
						}

						#ifndef DISABLEOCCLUSIONMAP
						cmpNStrConst( t_field_s, "occlusionmap" );
						if ( matched )
						{
							int index = atoi( t_field_s + 12 );
							if ( index < MAXMESHMATERIALS )
							{
								t.entityprofile[t.entid].WEMaterial.occlusionMapName[index] = t.value_s;
								t.entityprofile[t.entid].WEMaterial.MaterialActive = true;
							}
						}
						#endif

						cmpNStrConst( t_field_s, "emissivecolor" );
						if ( matched )
						{
							int index = atoi( t_field_s + 13 );
							if ( index < MAXMESHMATERIALS )
							{
								unsigned long ulValue = 0;
								sscanf(t.value_s.Get(), "%lu", &ulValue);
								t.entityprofile[t.entid].WEMaterial.dwEmmisiveColor[index] = ulValue;
								t.entityprofile[t.entid].WEMaterial.MaterialActive = true;
							}
						}

						cmpNStrConst( t_field_s, "basecolor" );
						if ( matched && t_field_s[9] != 'm' ) // filter out "basecolormap"
						{
							int index = atoi( t_field_s + 9 );
							if ( index < MAXMESHMATERIALS )
							{
								unsigned long ulValue = 0;
								sscanf(t.value_s.Get(), "%lu", &ulValue);
								t.entityprofile[t.entid].WEMaterial.dwBaseColor[index] = ulValue;
								t.entityprofile[t.entid].WEMaterial.MaterialActive = true;
							}
						}

						cmpNStrConst( t_field_s, "transparency" );
						if ( matched )
						{
							int index = atoi( t_field_s + 12 );
							if ( index < MAXMESHMATERIALS )
							{
								t.entityprofile[t.entid].WEMaterial.bTransparency[index] = t.value1_f;
								if (t.entityprofile[t.entid].WEMaterial.bTransparency[index] < 0)
									t.entityprofile[t.entid].WEMaterial.bTransparency[index] = 0;
							}

							//  transparency modes;
							//  0 - first-phase no alpha
							//  1 - first-phase with alpha masking
							//  2 and 3 - second-phase which overlaps solid geometry
							//  4 - alpha test (only render beyond 0x000000CF alpha values)
							//  5 - water Line (  object (seperates depth sort automatically) )
							//  6 - combination of 3 and 4 (second phase render with alpha blend AND alpha test, used for fading LOD leaves)
							//  7 - very early draw phase no alpha
							if ( t_field_s[12] == 0 )
							{
								if ( t.value1 == 5 ) t.value1 = 6; // 021215 - can only ben one water line
								t.entityprofile[t.entid].transparency = t.value1;
								if (t.entityprofile[t.entid].transparency < 0)
									t.entityprofile[t.entid].transparency = 0;
							}
						}

						cmpNStrConst( t_field_s, "doublesided" );
						if ( matched )
						{
							int index = atoi( t_field_s + 11 );
							if ( index < MAXMESHMATERIALS )
							{
								t.entityprofile[t.entid].WEMaterial.bDoubleSided[index] = t.value1_f;
							}
						}

						cmpNStrConst( t_field_s, "renderorderbias" );
						if ( matched )
						{
							int index = atoi( t_field_s + 15 );
							if ( index < MAXMESHMATERIALS )
							{
								t.entityprofile[t.entid].WEMaterial.fRenderOrderBias[index] = t.value1_f;
							}
						}

						cmpNStrConst( t_field_s, "castshadow" );
						if ( matched )
						{
							int index = atoi( t_field_s + 10 );
							if ( index < MAXMESHMATERIALS )
							{
								if ( t.value1_f == - 1 )
									t.entityprofile[t.entid].WEMaterial.bCastShadows[index] = false; 
								else
									t.entityprofile[t.entid].WEMaterial.bCastShadows[index] = true; 
							}
						}

						cmpNStrConst( t_field_s, "planerreflectio" );
						if ( matched )
						{
							int index = atoi( t_field_s + 16 );
							if ( index < MAXMESHMATERIALS )
							{
								t.entityprofile[t.entid].WEMaterial.bPlanerReflection[index] = t.value1_f; 
								t.entityprofile[t.entid].WEMaterial.MaterialActive = true;
							}
						}

						cmpNStrConst( t_field_s, "reflectance" );
						if ( matched )
						{
							int index = atoi( t_field_s + 11 );
							if ( index < MAXMESHMATERIALS )
							{
								t.entityprofile[t.entid].WEMaterial.fReflectance[index] = t.value1_f; 
								t.entityprofile[t.entid].WEMaterial.MaterialActive = true;
							}
						}
					}

					cmpStrConst( t_field_s, "effect" );
					if (  matched  )  t.entityprofile[t.entid].effect_s = t.value_s;

					// effectprofile:
					// 0 - default non-PBR
					// 1 - new PBR texture arrangement
					cmpStrConst( t_field_s, "effectprofile" );
					if ( matched ) t.entityprofile[t.entid].effectprofile = t.value1;

					cmpStrConst( t_field_s, "canseethrough" );
					if (  matched  )  t.entityprofile[t.entid].canseethrough = t.value1;

					// specular:
					// 0 - uses _S texture
					// 1 - uses effectbank\\reloaded\\media\\blank_none_S.dds
					// 2 - uses effectbank\\reloaded\\media\\blank_low_S.dds
					// 3 - uses effectbank\\reloaded\\media\\blank_medium_S.dds
					// 4 - uses effectbank\\reloaded\\media\\blank_high_S.dds
					cmpStrConst( t_field_s, "specular" );
					if (  matched  )  t.entityprofile[t.entid].specular = t.value1;

					// can scale the uv data inside the shader
					cmpStrConst( t_field_s, "uvscroll" );
					if (  matched  ) { t.entityprofile[t.entid].uvscrollu = t.value1/100.0f; t.entityprofile[t.entid].uvscrollv = t.value2/100.0f; }
					cmpStrConst( t_field_s, "uvscale" );
					if (  matched  ) { t.entityprofile[t.entid].uvscaleu = t.value1/100.0f; t.entityprofile[t.entid].uvscalev = t.value2/100.0f; }

					// can invert the normal, or set to zero to not invert (not inverted by default)
					cmpStrConst( t_field_s, "invertnormal" );
					if (  matched  )  t.entityprofile[t.entid].invertnormal = t.value1;

					// can choose whether to generate tangent/binormal in the shader
					cmpStrConst( t_field_s, "preservetangents" );
					if (  matched  )  t.entityprofile[t.entid].preservetangents = t.value1;
					
					cmpStrConst( t_field_s, "zdepth" );
					if (  matched  )  t.entityprofile[t.entid].zdepth = t.value1;

					cmpStrConst( t_field_s, "cullmode" );
					if (  matched  )  t.entityprofile[t.entid].cullmode = t.value1;
					cmpStrConst( t_field_s, "reducetexture" );
					if (  matched  )  t.entityprofile[t.entid].reducetexture = t.value1;

					// castshadow:
					//  0 = default shadow caster mode
					// -1 = do not cast shadows or lightmap shadows
					cmpStrConst( t_field_s, "castshadow" );
					if (  matched  )  t.entityprofile[t.entid].castshadow = t.value1;
					cmpStrConst( t_field_s, "smoothangle" );
					if (  matched  )  t.entityprofile[t.entid].smoothangle = t.value1;

					//  entity identity details
					cmpStrConst( t_field_s, "strength" );
					if (  matched  )  t.entityprofile[t.entid].strength = t.value1;
					cmpStrConst( t_field_s, "lives" );
					if (  matched  )  t.entityprofile[t.entid].lives = t.value1;
					cmpStrConst( t_field_s, "speed" );
					if (  matched  )  t.entityprofile[t.entid].speed = t.value1;
					cmpStrConst( t_field_s, "animspeed" );
					if (matched)
					{
						// LB: recent performance impprovements caused characters to move comically fast
						// so detect this specific default value and restore to a more reasonable speed
						// https://github.com/Dark-Basic-Software-Limited/GameGuruRepo/issues/6048
						if (t.entityprofile[t.entid].ischaracter == 1)
						{
							if (t.value1 == 100) t.value1 = 70;
						}

						// and of course any other value other than 100 will be the actual speed value set
						t.entityprofile[t.entid].animspeed = t.value1;
					}
					cmpStrConst( t_field_s, "hurtfall" );
					if (  matched  )  t.entityprofile[t.entid].hurtfall = t.value1;

					cmpStrConst( t_field_s, "isimmobile" );
					if (  matched  )  t.entityprofile[t.entid].isimmobile = t.value1;
					cmpStrConst( t_field_s, "isviolent" );
					if (  matched  )  t.entityprofile[t.entid].isviolent = t.value1;
					cmpStrConst( t_field_s, "isobjective" );
					if (  matched  )  t.entityprofile[t.entid].isobjective = t.value1;
					cmpStrConst( t_field_s, "alwaysactive" );
					if (  matched  )  t.entityprofile[t.entid].phyalways = t.value1;

					cmpStrConst(t_field_s, "iscollectable");
					if (matched)  t.entityprofile[t.entid].iscollectable = t.value1;
					cmpStrConst(t_field_s, "collectimage");
					if (matched)  t.entityprofile[t.entid].collectable.image = t.value_s;
					cmpStrConst(t_field_s, "collectdescription");
					if (matched)  t.entityprofile[t.entid].collectable.description = t.value_s;
					cmpStrConst(t_field_s, "collectcost");
					if (matched)  t.entityprofile[t.entid].collectable.cost = t.value1;
					cmpStrConst(t_field_s, "collectvalue");
					if (matched)  t.entityprofile[t.entid].collectable.value = t.value1;
					cmpStrConst(t_field_s, "collectcontainer");
					if (matched)  t.entityprofile[t.entid].collectable.container = t.value_s;
					cmpStrConst(t_field_s, "collectingredients");
					if (matched)  t.entityprofile[t.entid].collectable.ingredients = t.value_s;
					cmpStrConst(t_field_s, "collectstyle");
					if (matched)  t.entityprofile[t.entid].collectable.style = t.value_s;

					cmpStrConst( t_field_s, "ischaracter" );
					if (  matched  )  t.entityprofile[t.entid].ischaracter = t.value1;
					cmpStrConst( t_field_s, "isspinetracker" );
					if (  matched  )  t.entityprofile[t.entid].isspinetracker = t.value1;
					
					cmpStrConst( t_field_s, "noxzrotation" );
					if (  matched  )  t.entityprofile[t.entid].noXZrotation = t.value1;				
					cmpStrConst( t_field_s, "canfight" );
					if (  matched  )  t.entityprofile[t.entid].canfight = t.value1;

					cmpStrConst( t_field_s, "jumpmodifier" );
					if (  matched  )  t.entityprofile[t.entid].fJumpModifier = (float)t.value1 / 100.0f;
					cmpStrConst( t_field_s, "jumphold" );
					if (  matched  )  t.entityprofile[t.entid].jumphold = t.value1;
					cmpStrConst( t_field_s, "jumpresume" );
					if (  matched  )  t.entityprofile[t.entid].jumpresume = t.value1;
					cmpStrConst( t_field_s, "jumpvaulttrim" );
					if (  matched  )  t.entityprofile[t.entid].jumpvaulttrim = t.value1;

					cmpStrConst( t_field_s, "meleerange" );
					if (  matched  )  t.entityprofile[t.entid].meleerange = t.value1;
					cmpStrConst( t_field_s, "meleehitangle" );
					if (  matched  )  t.entityprofile[t.entid].meleehitangle = t.value1;
					cmpStrConst( t_field_s, "meleestrikest" );
					if (  matched  )  t.entityprofile[t.entid].meleestrikest = t.value1;
					cmpStrConst( t_field_s, "meleestrikefn" );
					if (  matched  )  t.entityprofile[t.entid].meleestrikefn = t.value1;
					cmpStrConst( t_field_s, "meleedamagest" );
					if (  matched  )  t.entityprofile[t.entid].meleedamagest = t.value1;
					cmpStrConst( t_field_s, "meleedamagefn" );
					if (  matched  )  t.entityprofile[t.entid].meleedamagefn = t.value1;

					cmpStrConst( t_field_s, "custombiped" );
					if (  matched  )  t.entityprofile[t.entid].custombiped = t.value1;

					cmpStrConst( t_field_s, "cantakeweapon" );
					if (  matched  )  t.entityprofile[t.entid].cantakeweapon = t.value1;
					cmpStrConst( t_field_s, "isweapon" );
					if (  matched  )  t.entityprofile[t.entid].isweapon_s = t.value_s;
					cmpStrConst( t_field_s, "rateoffire" );
					if (  matched  )  t.entityprofile[t.entid].rateoffire = t.value1;
					cmpStrConst( t_field_s, "ishudlayer" );
					if (  matched  )  t.entityprofile[t.entid].ishudlayer_s = t.value_s;

					//  fpgc - same internals just sanitized  -NEED TO GO IN EDITOR TOO!
					cmpStrConst( t_field_s, "isequipment" );
					if (  matched ) 
					{
						t.entityprofile[t.entid].isweapon_s=t.value_s;
						if (  Len(t.value_s.Get())>2 ) 
						{
							//  FPGC - 280809 - if equipment specified, this entity is ALWAYS ACTIVE (so can pickup AND DROP the item)
							t.entityprofile[t.entid].phyalways=1;
						}
					}

					cmpStrConst( t_field_s, "isammo" );
					if (  matched  )  t.entityprofile[t.entid].isammo = t.value1;
					cmpStrConst( t_field_s, "hasweapon" );
					if (  matched  )  t.entityprofile[t.entid].hasweapon_s = t.value_s;
					cmpStrConst(t_field_s, "ammopool");
					if (matched)  
						t.entityprofile[t.entid].ammopool_s = t.value_s;
					cmpStrConst( t_field_s, "hasequipment" );
					if (  matched  )  t.entityprofile[t.entid].hasweapon_s = t.value_s;
					cmpStrConst( t_field_s, "ishealth" );
					if (  matched  )  t.entityprofile[t.entid].ishealth = t.value1;
					cmpStrConst( t_field_s, "isflak" );
					if (  matched  )  t.entityprofile[t.entid].isflak = t.value1;
					cmpStrConst( t_field_s, "fatness" );
					if (  matched  )  t.entityprofile[t.entid].fatness = t.value1;

					//  marker extras
					//  1=player
					//  2=lights
					//  3=trigger zone
					//  4=decal particle emitter
					//  5=entity lights
					//  6=checkpoint zone
					//  7=multiplayer start
					//  8=floor zone
					//  9=cover zone
					//  10=new particle emitter (particle editor export)
					//  11=flag
					cmpStrConst( t_field_s, "ismarker" );
					if (  matched  )  
					{
						t.entityprofile[t.entid].ismarker = t.value1;
						if ( t.entityprofile[t.entid].ismarker > 0 && t.entityprofile[t.entid].ismarker != 2 )
						{
							// force zone and arrow markers to rise above terrain if planted there
							t.entityprofile[t.entid].offyoverride = 1;
							t.entityprofile[t.entid].offy = 2;
						}
					}
					cmpStrConst( t_field_s, "markerindex" );
					if (  matched  )  t.entityprofile[t.entid].markerindex = t.value1;
					cmpStrConst( t_field_s, "addhandle" );
					if (  matched  )  t.entityprofile[t.entid].addhandle_s = t.value_s;

					// ebe builder extras
					cmpStrConst( t_field_s, "isebe" );
					if (  matched  )  t.entityprofile[t.entid].isebe = t.value1;

					//  light extras
					cmpStrConst( t_field_s, "lightcolor" );
					if (  matched  )  t.entityprofile[t.entid].light.color = t.value1;
					cmpStrConst( t_field_s, "lightrange" );
					if (  matched  )  t.entityprofile[t.entid].light.range = t.value1;
					cmpStrConst( t_field_s, "lightoffsetup" );
					if (  matched  )  t.entityprofile[t.entid].light.offsetup = t.value1;
					cmpStrConst( t_field_s, "lightoffsetz" );
					if (  matched  )  t.entityprofile[t.entid].light.offsetz = t.value1;
					cmpStrConst( t_field_s, "lightprobescale" );
					if (matched)
					{
						t.entityprofile[t.entid].light.fLightHasProbe = t.value1;
						t.entityprofile[t.entid].light.fLightHasProbeX = t.value1;
						t.entityprofile[t.entid].light.fLightHasProbeY = t.value1;
						t.entityprofile[t.entid].light.fLightHasProbeZ = t.value1;
					}

					// light type flags
					cmpStrConst( t_field_s, "usespotlighting" );
					if (  matched  )  t.entityprofile[t.entid].usespotlighting = t.value1;

					//  trigger extras
					cmpStrConst( t_field_s, "stylecolor" );
					if (  matched  )  t.entityprofile[t.entid].trigger.stylecolor = t.value1;

					//  extra decal offset (ideal for placing flames in torches, etc)
					cmpStrConst( t_field_s, "decalangle" );
					if (  matched  )  t.entityprofile[t.entid].decaloffsetangle = t.value1;
					cmpStrConst( t_field_s, "decaldist" );
					if (  matched  )  t.entityprofile[t.entid].decaloffsetdist = t.value1/10.0;
					cmpStrConst( t_field_s, "decaly" );
					if (  matched  )  t.entityprofile[t.entid].decaloffsety = t.value1/10.0;

					//  entity body part list (20/01/11 - refeatured for V118)
					cmpStrConst( t_field_s, "limbmax" );
					if (  matched  )  
					{
						t.entityprofile[t.entid].limbmax = t.value1;
						if (  t.entityprofile[t.entid].limbmax>100  )  t.entityprofile[t.entid].limbmax = 100;
					}

					if (  t.entityprofile[t.entid].limbmax>0 ) 
					{
						cmpNStrConst( t_field_s, "limb" );
						if (  matched  )  
						{
							int limbNum = atoi( t_field_s+4 );
							if ( limbNum < t.entityprofile[t.entid].limbmax )
							{
								if ( limbNum == 0 )
								{
									if ( t_field_s[4] == '0' && t_field_s[5] == 0 ) t.entitybodypart[t.entid][ limbNum ] = t.value1;
								}
								else if ( limbNum < 10 )
								{
									if ( t_field_s[5] == 0 ) t.entitybodypart[t.entid][ limbNum ] = t.value1;
								}
								else if ( limbNum < 100 )
								{
									if ( t_field_s[6] == 0 ) t.entitybodypart[t.entid][ limbNum ] = t.value1;
								}
								else if ( limbNum < 1000 )
								{
									if ( t_field_s[7] == 0 ) t.entitybodypart[t.entid][ limbNum ] = t.value1;
								}
							}
						}
						matched = false; // prevent skipping other strings that start with "limb"
					}

					// read in head and spine tracker settings for this character (if any)
					cmpStrConst( t_field_s, "headhlimit" ); if (matched) { t.entityprofile[t.entid].headspinetracker.headhlimit = t.value1; }
					cmpStrConst( t_field_s, "headhoffset" ); if (matched) { t.entityprofile[t.entid].headspinetracker.headhoffset = t.value1; }
					cmpStrConst( t_field_s, "headvlimit" ); if (matched) { t.entityprofile[t.entid].headspinetracker.headvlimit = t.value1; }
					cmpStrConst( t_field_s, "headvoffset" ); if (matched) { t.entityprofile[t.entid].headspinetracker.headvoffset = t.value1; }
					cmpStrConst( t_field_s, "spinehlimit" ); if (matched) { t.entityprofile[t.entid].headspinetracker.spinehlimit = t.value1; }
					cmpStrConst( t_field_s, "spinehoffset" ); if (matched) { t.entityprofile[t.entid].headspinetracker.spinehoffset = t.value1; }
					cmpStrConst( t_field_s, "spinevlimit" ); if (matched) { t.entityprofile[t.entid].headspinetracker.spinevlimit = t.value1; }
					cmpStrConst( t_field_s, "spinevoffset" ); if (matched) { t.entityprofile[t.entid].headspinetracker.spinevoffset = t.value1; }

					//  determine if entity has a head, and which limbs represent it
					cmpStrConst( t_field_s, "headlimbs" );
					if (matched) { t.entityprofile[t.entid].headframestart = t.value1; t.entityprofile[t.entid].headframefinish = t.value2; }

					//  determine if entity has hair, and which limbs represent it/them
					cmpStrConst( t_field_s, "hairlimbs" );
					if (  matched ) { t.entityprofile[t.entid].hairframestart = t.value1; t.entityprofile[t.entid].hairframefinish = t.value2; }

					//  determine if entity has limbs to hide
					cmpStrConst( t_field_s, "hidelimbs" );
					if (  matched ) { t.entityprofile[t.entid].hideframestart = t.value1; t.entityprofile[t.entid].hideframefinish = t.value2; }

					//  entity decal refs
					cmpStrConst( t_field_s, "decalmax" );
					if (  matched  )
						t.entityprofile[t.entid].decalmax = t.value1;
					if (  t.entityprofile[t.entid].decalmax>0 ) 
					{
						cmpNStrConst( t_field_s, "decal" );
						if (  matched  )  
						{
							const int rootindex = 5;
							bool bValid = false;
							int index = atoi( t_field_s+rootindex );
							if ( index == 0 )
							{
								if ( t_field_s[rootindex] == '0' && t_field_s[rootindex+1] == 0 ) bValid = true;
							}
							else if ( index < 10 )
							{
								if ( t_field_s[rootindex+1] == 0 ) bValid = true;
							}
							else if ( index < 100 )
							{
								if ( t_field_s[rootindex+2] == 0 ) bValid = true;
							}
							else if ( index < 1000 )
							{
								if ( t_field_s[rootindex+3] == 0 ) bValid = true;
							}
							else if ( index < 10000 )
							{
								if ( t_field_s[rootindex+4] == 0 ) bValid = true;
							}

							if ( bValid && index < t.entityprofile[t.entid].decalmax )
							{
								t.entitydecal_s[t.entid][index] = t.value_s;
							}
						}
					}

					// 060718 - entity append anim system
					cmpStrConst( t_field_s, "appendanimfinal" );
					if (  matched )
					{ 
						t.entityappendanim[t.entid][0].filename = t.value_s; 
						t.entityappendanim[t.entid][0].startframe = 0;
					}
					cmpStrConst( t_field_s, "appendanimmax" );
					if ( matched ) 
					{
						t.entityprofile[t.entid].appendanimmax = t.value1; 
						if ( t.entityprofile[t.entid].appendanimmax > 99 ) 
							t.entityprofile[t.entid].appendanimmax = 99;
					}

					//PE: Hanging, in my case: appendanimmax=573444874 value_s=road_straight01.x
					//PE: Hang if you are unlucky and get mem that "appendanimmax" are not already set to zero.
					if ( t.entityprofile[t.entid].appendanimmax > 0 && t.entityprofile[t.entid].appendanimmax <= 99 )
					{
						cmpNStrConst( t_field_s, "appendanimframe" );
						if (  matched  )  
						{
							int index = atoi( t_field_s+15 );
							if ( index != 0 && index < t.entityprofile[t.entid].appendanimmax )
							{
								if ( index < 10 )
								{
									if ( t_field_s[16] == 0 ) t.entityappendanim[t.entid][index].filename = t.value_s;
								}
								else if ( index < 100 )
								{
									if ( t_field_s[17] == 0 ) t.entityappendanim[t.entid][index].filename = t.value_s;
								}
								else if ( index < 1000 )
								{
									if ( t_field_s[18] == 0 ) t.entityappendanim[t.entid][index].filename = t.value_s;
								}
							}
						}
						else
						{
							cmpNStrConst( t_field_s, "appendanim" );
							if (  matched  )  
							{
								int index = atoi( t_field_s+10 );
								if ( index != 0 && index < t.entityprofile[t.entid].appendanimmax )
								{
									if ( index < 10 )
									{
										if ( t_field_s[11] == 0 ) t.entityappendanim[t.entid][index].filename = t.value_s;
									}
									else if ( index < 100 )
									{
										if ( t_field_s[12] == 0 ) t.entityappendanim[t.entid][index].filename = t.value_s;
									}
									else if ( index < 1000 )
									{
										if ( t_field_s[13] == 0 ) t.entityappendanim[t.entid][index].filename = t.value_s;
									}
								}
							}
						}
					}

					cmpStrConst( t_field_s, "drawcalloptimizer" );
					if (matched)  t.entityprofile[t.entid].drawcalloptimizer = t.value1;
					cmpStrConst( t_field_s, "drawcalloptimizeroff" );
					if (matched)  t.entityprofile[t.entid].drawcalloptimizeroff = t.value1;
					cmpStrConst( t_field_s, "drawcallscaleadjust" );
					if (matched)  t.entityprofile[t.entid].drawcallscaleadjust = t.value1;


					//  entity animation sets
					cmpStrConst( t_field_s, "ignorecsirefs" );
					if (  matched  )  t.entityprofile[t.entid].ignorecsirefs = t.value1;
					cmpStrConst( t_field_s, "playanimineditor" );
					if (matched)
					{
						// work out correct anim to play in editor
						int iRealPlayAnimInEditorIndex = t.value1;
						LPSTR pNumString = t.value_s.Get();
						int iIsAPureNumeric = 1;
						for (int n = 0; n < strlen(pNumString); n++)
						{
							if (pNumString[n] >= '0' && pNumString[n] <= '9')
							{
								// can have numbers in a name string
							}
							else
							{
								// found a non number, cannot be a numeric
								iIsAPureNumeric = 0;
								break;
							}
						}
						if (iIsAPureNumeric == 0 && strlen(t.value_s.Get())>0)
						{
							// not a numeric, is an anim name we can search for	
							t.entityprofile[t.entid].playanimineditor_name = t.value_s;
							t.entityprofile[t.entid].playanimineditor = -1;
							t.entityprofile[t.entid].startanimingame = 0;
						}
						else
						{
							// editor uses this to play
							t.entityprofile[t.entid].playanimineditor = iRealPlayAnimInEditorIndex;
							t.entityprofile[t.entid].playanimineditor_name = "";
							// startanimingame is still used in standalone.
							t.entityprofile[t.entid].startanimingame = iRealPlayAnimInEditorIndex;
						}
					}
					cmpStrConst( t_field_s, "animstyle" );
					if (  matched  )  
						t.entityprofile[t.entid].animstyle = t.value1;
					cmpStrConst( t_field_s, "animmax" );
					if (  matched ) 
					{
						t.tnewanimmax=t.value1 ; t.tstartofaianim=t.tnewanimmax;
					}
					if (  t.tnewanimmax>0 ) 
					{
						cmpNStrConst( t_field_s, "anim" );
						if (  matched  )  
						{
							const int rootindex = 4;
							int index = atoi( t_field_s+rootindex );
							bool bValid = false;
							if ( index == 0 )
							{
								if ( t_field_s[rootindex] == '0' && t_field_s[rootindex+1] == 0 ) bValid = true;
							}
							else if ( index < 10 )
							{
								if ( t_field_s[rootindex+1] == 0 ) bValid = true;
							}
							else if ( index < 100 )
							{
								if ( t_field_s[rootindex+2] == 0 ) bValid = true;
							}
							else if ( index < 1000 )
							{
								if ( t_field_s[rootindex+3] == 0 ) bValid = true;
							}
							else if ( index < 10000 )
							{
								if ( t_field_s[rootindex+4] == 0 ) bValid = true;
							}

							if ( bValid && index < t.tstartofaianim )
							{
								t.entityanim[t.entid][index].start = t.value1 ; 
								t.entityanim[t.entid][index].finish = t.value2 ; 
								t.entityanim[t.entid][index].found = 1;
							}
						}
						matched = false; // prevent skipping other strings that start with "anim"
					}

					// get foot fall data (optional)
					cmpStrConst( t_field_s, "footfallmax" );
					if (  matched  )  t.entityprofile[t.entid].footfallmax = t.value1;
					if (  t.entityprofile[t.entid].footfallmax>0 ) 
					{
						cmpNStrConst( t_field_s, "footfall" );
						if (  matched  )  
						{
							const int rootindex = 8;
							int index = atoi( t_field_s+rootindex );
							bool bValid = false;
							if ( index == 0 )
							{
								if ( t_field_s[rootindex] == '0' && t_field_s[rootindex+1] == 0 ) bValid = true;
							}
							else if ( index < 10 )
							{
								if ( t_field_s[rootindex+1] == 0 ) bValid = true;
							}
							else if ( index < 100 )
							{
								if ( t_field_s[rootindex+2] == 0 ) bValid = true;
							}
							else if ( index < 1000 )
							{
								if ( t_field_s[rootindex+3] == 0 ) bValid = true;
							}
							else if ( index < 10000 )
							{
								if ( t_field_s[rootindex+4] == 0 ) bValid = true;
							}

							if ( bValid && index < t.entityprofile[t.entid].footfallmax )
							{
								t.entityfootfall[t.entid][index].leftfootkeyframe = t.value1; 
								t.entityfootfall[t.entid][index].rightfootkeyframe = t.value2;
							}
						}
					}

					//  more data
					cmpStrConst( t_field_s, "quantity" );
					if (  matched  )  t.entityprofile[t.entid].quantity = t.value1;

					//  physics objects from the importer
					cmpStrConst( t_field_s, "physicscount" );
					if (  matched  )  t.entityprofile[t.entid].physicsobjectcount = t.value1;

					if (  cstr(Left(t.field_s.Get(),7)) == "physics" && t.field_s != "physicscount" && t.tPhysObjCount < MAX_ENTITY_PHYSICS_BOXES ) 
					{

						Dim (  t.tArray,10 );

							//  get rid of the quotation marks
							t.tStrip_s = t.value_s;
							t.tStrip_s = Left(t.tStrip_s.Get(), Len(t.tStrip_s.Get())-1);
							t.tStrip_s = Right(t.tStrip_s.Get(), Len(t.tStrip_s.Get())-1);

							t.tArrayMarker = 0;
							t.ttToken_s=FirstToken(t.tStrip_s.Get(),",");
							//PE: Make sure we only run it if we find a token.
							//PE: https://github.com/TheGameCreators/GameGuruRepo/issues/979

							if (t.ttToken_s != "")
							{
								t.tArray[t.tArrayMarker] = t.ttToken_s;
								++t.tArrayMarker;

								do
								{
									t.ttToken_s = NextToken(",");
									if (t.ttToken_s != "")
									{
										t.tArray[t.tArrayMarker] = t.ttToken_s;
										++t.tArrayMarker;
									}
								} while (!(t.ttToken_s == ""));

								//  Format; shapetype, sizex, sizey, sizez, offx, offy, offz, rotx, roty, rotz
								t.tPShapeType = ValF(t.tArray[0].Get());
								//  is it a box?
								if (t.tPShapeType == 0)
								{
									//Dave Crash fix - check we are not going out of bounds
									if (t.entid < MAX_ENTITY_PHYSICS_BOXES * 2)
									{
										t.entityphysicsbox[t.entid][t.tPhysObjCount].SizeX = ValF(t.tArray[1].Get());
										t.entityphysicsbox[t.entid][t.tPhysObjCount].SizeY = ValF(t.tArray[2].Get());
										t.entityphysicsbox[t.entid][t.tPhysObjCount].SizeZ = ValF(t.tArray[3].Get());
										t.entityphysicsbox[t.entid][t.tPhysObjCount].OffX = ValF(t.tArray[4].Get());
										t.entityphysicsbox[t.entid][t.tPhysObjCount].OffY = ValF(t.tArray[5].Get());
										t.entityphysicsbox[t.entid][t.tPhysObjCount].OffZ = ValF(t.tArray[6].Get());
										t.entityphysicsbox[t.entid][t.tPhysObjCount].RotX = ValF(t.tArray[7].Get());
										t.entityphysicsbox[t.entid][t.tPhysObjCount].RotY = ValF(t.tArray[8].Get());
										t.entityphysicsbox[t.entid][t.tPhysObjCount].RotZ = ValF(t.tArray[9].Get());
									}
									++t.tPhysObjCount;
								}

							}

						UnDim (  t.tArray );

					}

				}
			}
		}
		UnDim (  t.data_s );

		for ( int i = 0 ; i <= t.tstartofaianim-1; i++ )
		{
			if ( t.entityanim[t.entid][i].found == 0 ) 
			{ 
				t.entityanim[t.entid][i].start = -1; 
				t.entityanim[t.entid][i].finish = -1; 
			}
		}

		if (t.entityprofile[t.entid].ismarker == 2) 
		{
			//PE: LMFIX - Light , default to 100.
			if (t.entityprofile[t.entid].defaultheight == 155)
				t.entityprofile[t.entid].defaultheight = 80;

			// special code to set this light to black but keep alpha
			if (t.entityprofile[t.entid].light.color == -100)
			{
				t.entityprofile[t.entid].light.color = (255 << 24);
			}
		}

		//  Finish animation quantities
		t.entityprofile[t.entid].animmax=t.tnewanimmax;
		t.entityprofile[t.entid].startofaianim=t.tstartofaianim;

		//  Localisation must change desc to local name
		if (  t.entityprofileheader[t.entid].desc_s != "" ) 
		{
			if (  cstr(Left(t.entityprofileheader[t.entid].desc_s.Get(),1)) != "%" ) 
			{
				// no LOC files in wicked for now
			}
		}

		//  Translate entity references inside entity profile (token translations)
		if (  cstr(Lower(t.entityprofileheader[t.entid].desc_s.Get())) == "%key" ) 
		{
			t.entityprofileheader[t.entid].desc_s=t.strarr_s[472];
		}
		if (  cstr(Lower(t.entityprofileheader[t.entid].desc_s.Get())) == "%light" ) 
		{
			t.entityprofileheader[t.entid].desc_s=t.strarr_s[473];
		}
		if (  cstr(Lower(t.entityprofileheader[t.entid].desc_s.Get())) == "%remote door" ) 
		{
			t.entityprofileheader[t.entid].desc_s=t.strarr_s[474];
		}
		if (  cstr(Lower(t.entityprofileheader[t.entid].desc_s.Get())) == "%teleporter in" ) 
		{
			t.entityprofileheader[t.entid].desc_s=t.strarr_s[615];
		}
		if (  cstr(Lower(t.entityprofileheader[t.entid].desc_s.Get())) == "%teleporter out" ) 
		{
			t.entityprofileheader[t.entid].desc_s=t.strarr_s[616];
		}
		if (  cstr(Lower(t.entityprofileheader[t.entid].desc_s.Get())) == "%lift" ) 
		{
			t.entityprofileheader[t.entid].desc_s=t.strarr_s[617];
		}
		if (  cstr(Lower(t.entityprofile[t.entid].usekey_s.Get())) == "%key" ) 
		{
			t.entityprofile[t.entid].usekey_s=t.strarr_s[472];
		}
		if (  cstr(Lower(t.entityprofile[t.entid].ifused_s.Get())) == "%light" ) 
		{
			t.entityprofile[t.entid].ifused_s=t.strarr_s[473];
		}
		if (  cstr(Lower(t.entityprofile[t.entid].ifused_s.Get())) == "%remote door" ) 
		{
			t.entityprofile[t.entid].ifused_s=t.strarr_s[474];
		}
		if (  cstr(Lower(t.entityprofile[t.entid].ifused_s.Get())) == "%teleporter in" ) 
		{
			t.entityprofile[t.entid].ifused_s=t.strarr_s[615];
		}
		if (  cstr(Lower(t.entityprofile[t.entid].ifused_s.Get())) == "%teleporter out" ) 
		{
			t.entityprofile[t.entid].ifused_s=t.strarr_s[616];
		}
		if (  cstr(Lower(t.entityprofile[t.entid].ifused_s.Get())) == "%lift" ) 
		{
			t.entityprofile[t.entid].ifused_s=t.strarr_s[617];
		}

		//  All profile defaults
		if (  t.entityprofile[t.entid].ismarker != 1 ) 
		{
			if (  t.entityprofile[t.entid].lives<1  )  t.entityprofile[t.entid].lives = 1;
		}
		if (  t.entityprofile[t.entid].speed == 0  )  t.entityprofile[t.entid].speed = 100;
		if (  t.entityprofile[t.entid].hurtfall == 0  )  t.entityprofile[t.entid].hurtfall = 100;

		//  Physics Data Defaults
		if (  t.entityprofile[t.entid].ismarker == 0 ) 
		{
			//  default physics settings (weight and friction done during object load (we need the obj size!)
			//  health packs have no physics by default for A compatibility
			if (  t.entityprofile[t.entid].ishealth != 0 ) 
			{
				t.entityprofile[t.entid].physics=0;
			}
		}
		else
		{
			t.entityprofile[t.entid].physics=0;
		}

		//LB: Additional assumption that objects with no collision should not have physics
		if (t.entityprofile[t.entid].collisionmode >= 11 && t.entityprofile[t.entid].collisionmode <= 12)
		{
			t.entityprofile[t.entid].physics = 0;
		}

		//  LOD System Defaults
		if ( t.entityprofile[t.entid].lod1distance > 0 && t.entityprofile[t.entid].lod2distance == 0 )
		{
			t.entityprofile[t.entid].lod2distance = t.entityprofile[t.entid].lod1distance;
		}

		//  Spawn defaults
		t.entityprofile[t.entid].spawnatstart=1;
		t.entityprofile[t.entid].spawndelayrandom=0;
		t.entityprofile[t.entid].spawnqtyrandom=0;
		t.entityprofile[t.entid].spawnvel=0;
		t.entityprofile[t.entid].spawnvelrandom=0;
		t.entityprofile[t.entid].spawnangle=0;
		t.entityprofile[t.entid].spawnanglerandom=0;
		t.entityprofile[t.entid].spawnlife=0;
		if (  t.entityprofile[t.entid].spawnmax>0 ) 
		{
			t.entityprofile[t.entid].spawnupto=t.entityprofile[t.entid].spawnmax;
			t.entityprofile[t.entid].spawnafterdelay=1;
			if (  t.entityprofile[t.entid].ischaracter == 1 ) 
			{
				t.entityprofile[t.entid].spawnwhendead=1;
			}
			else
			{
				t.entityprofile[t.entid].spawnwhendead=0;
			}
		}
		else
		{
			t.entityprofile[t.entid].spawnupto=0;
			t.entityprofile[t.entid].spawnafterdelay=0;
			t.entityprofile[t.entid].spawnwhendead=0;
		}

		//  Fix scale for FPE
		if (  t.entityprofile[t.entid].scale == 0 ) 
		{
			t.entityprofile[t.entid].scale=100;
		}

		// 010917 - if shader effect is a decal, auto switch zdepth flag (shader no longer does this in DX11)
		LPSTR pEffectMatch = "effectbank\\reloaded\\decal";
		if ( strnicmp ( t.entityprofile[t.entid].effect_s.Get(), pEffectMatch, strlen(pEffectMatch) ) == NULL ) 
		{
			t.entityprofile[t.entid].zdepth = 0;
		}

		// 261117 - intercept and replace any legacy shaders with new PBR ones if game visuals using RealtimePBR (3) mode
		if ( g.gpbroverride == 1 )
		{
			int iReplaceMode = 0;
			LPSTR pTryMatch = "effectbank\\reloaded\\entity_basic.fx";
			if ( strnicmp ( t.entityprofile[t.entid].effect_s.Get(), pTryMatch, strlen(pTryMatch) ) == NULL ) iReplaceMode = 1;
			pTryMatch = "effectbank\\reloaded\\character_static.fx";
			if ( strnicmp ( t.entityprofile[t.entid].effect_s.Get(), pTryMatch, strlen(pTryMatch) ) == NULL ) iReplaceMode = 1;
			pTryMatch = "effectbank\\reloaded\\entity_anim.fx";
			if ( strnicmp ( t.entityprofile[t.entid].effect_s.Get(), pTryMatch, strlen(pTryMatch) ) == NULL ) iReplaceMode = 2;
			pTryMatch = "effectbank\\reloaded\\character_basic.fx";
			if ( strnicmp ( t.entityprofile[t.entid].effect_s.Get(), pTryMatch, strlen(pTryMatch) ) == NULL ) iReplaceMode = 5;
			pTryMatch = "effectbank\\reloaded\\character_transparency.fx";
			if ( strnicmp ( t.entityprofile[t.entid].effect_s.Get(), pTryMatch, strlen(pTryMatch) ) == NULL ) iReplaceMode = 2;
			pTryMatch = "effectbank\\reloaded\\tree_basic.fx";
			if ( strnicmp ( t.entityprofile[t.entid].effect_s.Get(), pTryMatch, strlen(pTryMatch) ) == NULL ) iReplaceMode = 3;			
			pTryMatch = "effectbank\\reloaded\\treea_basic.fx";
			if ( strnicmp ( t.entityprofile[t.entid].effect_s.Get(), pTryMatch, strlen(pTryMatch) ) == NULL ) iReplaceMode = 4;	
			if ( strlen ( t.entityprofile[t.entid].effect_s.Get() ) == 0 ) iReplaceMode = 1;
			if ( iReplaceMode > 0 )
			{
				if ( iReplaceMode == 1 ) t.entityprofile[t.entid].effect_s = "effectbank\\reloaded\\apbr_basic.fx";
				if ( iReplaceMode == 2 ) t.entityprofile[t.entid].effect_s = "effectbank\\reloaded\\apbr_anim.fx";
				if ( iReplaceMode == 3 ) t.entityprofile[t.entid].effect_s = "effectbank\\reloaded\\apbr_tree.fx";
				if ( iReplaceMode == 4 ) t.entityprofile[t.entid].effect_s = "effectbank\\reloaded\\apbr_treea.fx";
				if ( iReplaceMode == 5 ) t.entityprofile[t.entid].effect_s = "effectbank\\reloaded\\apbr_animwithtran.fx";
			}
		}
		else
		{
			// 120418 - conversely, if PBR override not active, and have new PBR asset entities that still 
			// have old DNS textures, switch them back to classic non-PBR (this allows new PBR assets to 
			// replace older legacy assets but still allow backwards compatibility for users who want the
			// old shaders and old textures to remain in effect using PBR override of zero)
			char pEntityItemPath[1024];
			strcpy ( pEntityItemPath, t.ent_s.Get() );
			int n = 0;
			for ( n = strlen(pEntityItemPath)-1; n > 0; n-- )
			{
				if ( pEntityItemPath[n] == '\\' || pEntityItemPath[n] == '/' )
				{
					pEntityItemPath[n+1] = 0;
					break;
				}
			}
			if ( n <= 0 ) strcpy ( pEntityItemPath, "" );
			char pJustTextureName[1024];
			strcpy ( pJustTextureName, t.entityprofile[t.entid].texd_s.Get() );
			if ( strlen ( pJustTextureName ) > 4 )
			{
				pJustTextureName[strlen(pJustTextureName)-4]=0;
				if ( stricmp ( pJustTextureName+strlen(pJustTextureName)-6, "_color" ) == NULL )
				{
					pJustTextureName[strlen(pJustTextureName)-6]=0;
					strcat ( pJustTextureName, "_D" );
				}
				strcat ( pJustTextureName, ".png" );
			}
			char pReplaceWithDNS[1024];
			strcpy ( pReplaceWithDNS, pEntityItemPath );
			strcat ( pReplaceWithDNS, pJustTextureName );
			bool bReplacePBRWithNonPBRDNS = false;
			LPSTR pPBREffectMatch = "effectbank\\reloaded\\apbr";
			if ( strnicmp ( t.entityprofile[t.entid].effect_s.Get(), pPBREffectMatch, strlen(pPBREffectMatch) ) == NULL ) 
			{
				// entity effect specifies PBR, do we have the DNS files available
				if ( strlen ( pJustTextureName ) > 4 )
				{
					cstr pFindDNSFile = t.entdir_s + pReplaceWithDNS;
					if ( FileExist ( pFindDNSFile.Get() ) == 0 )
					{
						pReplaceWithDNS[strlen(pReplaceWithDNS)-4]=0;
						strcat ( pReplaceWithDNS, ".dds" );
						pFindDNSFile = t.entdir_s + pReplaceWithDNS;
						if ( FileExist ( pFindDNSFile.Get() ) == 0 )
						{
							pReplaceWithDNS[strlen(pReplaceWithDNS)-4]=0;
							strcat ( pReplaceWithDNS, ".jpg" );
							pFindDNSFile = t.entdir_s + pReplaceWithDNS;
							if ( FileExist ( pFindDNSFile.Get() ) == 1 )
							{
								bReplacePBRWithNonPBRDNS = true;
							}
						}
						else
						{
							bReplacePBRWithNonPBRDNS = true;
						}
					}
					else
					{
						bReplacePBRWithNonPBRDNS = true;
					}
				}
				else
				{
					// no texture specified, but can still switch to classic shaders (legacy behavior)
					bReplacePBRWithNonPBRDNS = true;
				}
			}
			if ( bReplacePBRWithNonPBRDNS == true )
			{
				// replace the shader used
				int iReplaceMode = 0;
				LPSTR pTryMatch = "effectbank\\reloaded\\apbr_basic.fx";
				if ( strnicmp ( t.entityprofile[t.entid].effect_s.Get(), pTryMatch, strlen(pTryMatch) ) == NULL ) iReplaceMode = 1;
				pTryMatch = "effectbank\\reloaded\\apbr_anim.fx";
				if ( strnicmp ( t.entityprofile[t.entid].effect_s.Get(), pTryMatch, strlen(pTryMatch) ) == NULL ) iReplaceMode = 2;
				pTryMatch = "effectbank\\reloaded\\apbr_tree.fx";
				if ( strnicmp ( t.entityprofile[t.entid].effect_s.Get(), pTryMatch, strlen(pTryMatch) ) == NULL ) iReplaceMode = 3;
				pTryMatch = "effectbank\\reloaded\\apbr_treea.fx";
				if ( strnicmp ( t.entityprofile[t.entid].effect_s.Get(), pTryMatch, strlen(pTryMatch) ) == NULL ) iReplaceMode = 4;
				if ( iReplaceMode > 0 )
				{
					if ( iReplaceMode == 1 ) t.entityprofile[t.entid].effect_s = "effectbank\\reloaded\\entity_basic.fx";
					if ( iReplaceMode == 2 ) t.entityprofile[t.entid].effect_s = "effectbank\\reloaded\\character_basic.fx";
					if ( iReplaceMode == 3 ) t.entityprofile[t.entid].effect_s = "effectbank\\reloaded\\tree_basic.fx";
					if ( iReplaceMode == 4 ) t.entityprofile[t.entid].effect_s = "effectbank\\reloaded\\treea_basic.fx";
				}

				// replace the texture specified (from _color to _D)
				t.entityprofile[t.entid].texd_s = pJustTextureName;
			}
		}

		// if effect shader starts with APBR, auto shift effectprofile from zero to one
		LPSTR pPBREffectMatch = "effectbank\\reloaded\\apbr";
		if ( strnicmp ( t.entityprofile[t.entid].effect_s.Get(), pPBREffectMatch, strlen(pPBREffectMatch) ) == NULL ) 
		{
			if ( t.entityprofile[t.entid].effectprofile == 0 )
				t.entityprofile[t.entid].effectprofile = 1;
		}
	}

	//  Can loop back if skipBIN flag set
	if (  t.skipBINloadingandtryagain == 1 ) 
	{
		timestampactivity(0,cstr(cstr(Str(t.tprotectBINfile))+" Entity BIN File Out Of Date: "+t.tprofile_s).Get());
		if (  t.game.gameisexe == 1 ) 
		{
			t.skipBINloadingandtryagain=0;
		}
		else
		{
			if (  t.tprotectBINfile == 0 ) 
			{
				if (  FileExist(t.tprofile_s.Get()) == 1  )  DeleteAFile (  t.tprofile_s.Get() );
			}
			else
			{
				t.skipBINloadingandtryagain=0;
			}
		}
	}

	} while ( !(  t.skipBINloadingandtryagain == 0 ) );

	//  new field as we now have pure lights and entity lights
	if (  t.entityprofile[t.entid].ismarker == 2 || t.entityprofile[t.entid].ismarker == 5 ) 
	{
		if (  t.entityprofile[t.entid].ismarker == 5  )  t.entityprofile[t.entid].ismarker = 0;
		t.entityprofile[t.entid].islightmarker=1;
		//  FPGC - 300310 - entitylights always active as they may control a dynamic light and possibly decal-particle(mode7)
		t.entityprofile[t.entid].phyalways=1;
	}
	else
	{
		t.entityprofile[t.entid].islightmarker=0;
	}

	//  FPGC - 100610 - all FPGC characters are ALWAYS ACTIVE for full speed logic (more predictable)
	if (  t.entityprofile[t.entid].ischaracter == 1 && g.fpgcgenre == 0 ) 
	{
		t.entityprofile[t.entid].phyalways=1;
		//  FPGC - 110610 - and ALL are invincible
		t.entityprofile[t.entid].strength=0;
	}

	if (t.entityprofile[t.entid].ischaracter == 0)
	{
		// only characters can use isspinetracker mode (now on by default for wicked)
		t.entityprofile[t.entid].isspinetracker = 0;
	}

	//  fileexistelse
	}
	else
	{
		//  File not exist, provide debug information (only if file specified (old entities can be renamed and still hang around inside FPMs)
		if (  Len( cstr(t.entdir_s+t.ent_s).Get() )>Len("entitybank\\") ) 
		{
			debugfilename( cstr(t.entdir_s+t.ent_s).Get(),t.tprofile_s.Get() );
		}
	}

	//  V109 BETA5 - 250408 - flag material is being used
	if (  t.entityprofile[t.entid].materialindex>0 ) 
	{
		//t.mi=t.entityprofile[t.entid].materialindex-1;
		t.mi = t.entityprofile[t.entid].materialindex; //PE: We now use the correct materialindex -1 is from classic.
		if (t.mi < t.material.size())
		{
			t.material[t.mi].usedinlevel = 1;
		}
		else
		{
			// speciried a material index that MAX does not support!
		}
	}

	// if flagged as EBE, attempt to load any EBE cube data
	if ( t.entityprofile[t.entid].isebe != 0 )
	{
		cstr sEBEFile = cstr(Left(t.tFPEName_s.Get(),strlen(t.tFPEName_s.Get())-4)) + cstr(".ebe");
		if ( FileExist ( sEBEFile.Get() ) ) 
		{
			// load EBE data into entityID
			ebe_load_ebefile ( sEBEFile, t.entid );
		}
		else
		{
			// 300817 - EBE has had .ebe file removed to make it regular entity, so remove handle limb
			if ( t.entityprofile[t.entid].isebe == 1 ) 
			{
				t.entityprofile[t.entid].isebe = 2;
			}
		}
	}

	// set transparency for all markers
	if (t.entityprofile[t.entid].ismarker > 0 && t.entityprofile[t.entid].ismarker != 11)
		t.entityprofile[t.entid].transparency = 6;

	#ifndef GGMAXEDU
	// also, objects may reference old scripts, now can look for their new locations via Workshop items
	extern bool workshop_verifyandorreplacescript(int, int);
	if (workshop_verifyandorreplacescript(0, t.entid) == true)
	{
		// we replaced this script with one that exists elsewhere :)
	}
	#endif
}

void entity_loadvideoid ( void )
{
	t.tvideoid=0;
	t.text_s = Lower(Right(t.tvideofile_s.Get(),4));
	if ( t.text_s == ".ogv" || t.text_s == ".mp4" ) 
	{
		t.tvideoid=32;
		for ( t.tt = 1 ; t.tt<=  32; t.tt++ )
		{
			if ( AnimationExist(t.tt) == 0 ) { t.tvideoid = t.tt  ; break; }
		}
		char pFinalVideoFilePath[MAX_PATH];
		strcpy(pFinalVideoFilePath, t.tvideofile_s.Get());
		GG_GetRealPath(pFinalVideoFilePath, 0);
		if ( LoadAnimation(pFinalVideoFilePath, t.tvideoid, g.videoprecacheframes, g.videodelayedload, 1) == false )
		{
			t.tvideoid = -999;
		}
	}
}

void entity_loadactivesoundsandvideo ( void )
{
	// sounds in each entity
	char pSoundSet0[MAX_PATH];
	char pSoundSet1[MAX_PATH];
	char pSoundSet2[MAX_PATH];
	char pSoundSet3[MAX_PATH];
	char pSoundSet4[MAX_PATH];
	char pSoundSet5[MAX_PATH];
	char pSoundSet6[MAX_PATH];

	// go through all entities in level
	for ( t.e = 1 ; t.e <= g.entityelementlist; t.e++ )
	{
		t.entid=t.entityelement[t.e].bankindex;
		if ( t.entid>0 ) 
		{
			bool bNeedAssetsLoading = false;
			if (t.entityelement[t.e].active == 1) bNeedAssetsLoading = true;
			if (t.entityelement[t.e].eleprof.spawnatstart == 0) bNeedAssetsLoading = true;
			if ( bNeedAssetsLoading == true )
			{
				// original base filenames for sound
				strcpy(pSoundSet0, t.entityelement[t.e].eleprof.soundset_s.Get());
				strcpy(pSoundSet1, t.entityelement[t.e].eleprof.soundset1_s.Get());
				strcpy(pSoundSet2, t.entityelement[t.e].eleprof.soundset2_s.Get());
				strcpy(pSoundSet3, t.entityelement[t.e].eleprof.soundset3_s.Get());
				strcpy(pSoundSet4, t.entityelement[t.e].eleprof.soundset4a_s.Get());
				strcpy(pSoundSet5, t.entityelement[t.e].eleprof.soundset5_s.Get());
				strcpy(pSoundSet6, t.entityelement[t.e].eleprof.soundset6_s.Get());

				// new system can adjust sound at load time to provide automatic variances
				if (t.entityprofile[t.entid].ischaracter != 0 && t.entityelement[t.e].eleprof.iUseSoundVariants)
				{
					// only apply variant system to characters (for now to limit additional setup time)
					for (int allfour = 0; allfour <= 6; allfour++)
					{
						bool bMightHaveVariant = false;
						LPSTR pThisStr = NULL;
						if (allfour == 0) pThisStr = pSoundSet0;
						if (allfour == 1) pThisStr = pSoundSet1;
						if (allfour == 2) pThisStr = pSoundSet2;
						if (allfour == 3) pThisStr = pSoundSet3;
						if (allfour == 4) pThisStr = pSoundSet4;
						if (allfour == 5) pThisStr = pSoundSet5;
						if (allfour == 6) pThisStr = pSoundSet6;
						if (bMightHaveVariant == false && pThisStr && strnicmp (pThisStr + strlen(pThisStr) - 5, "1.wav", 5) == NULL) bMightHaveVariant = true;
						if (bMightHaveVariant == false && pThisStr && strnicmp (pThisStr + strlen(pThisStr) - 5, "2.wav", 5) == NULL) bMightHaveVariant = true;
						if (bMightHaveVariant == false && pThisStr && strnicmp (pThisStr + strlen(pThisStr) - 5, "3.wav", 5) == NULL) bMightHaveVariant = true;
						if (bMightHaveVariant == false && pThisStr && strnicmp (pThisStr + strlen(pThisStr) - 5, "4.wav", 5) == NULL) bMightHaveVariant = true;
						if (bMightHaveVariant == false && pThisStr && strnicmp (pThisStr + strlen(pThisStr) - 5, "5.wav", 5) == NULL) bMightHaveVariant = true;
						if (bMightHaveVariant == true)
						{
							int iOriginal = pThisStr[strlen(pThisStr) - 5] - '1';
							int iRnd = rand() % 5;
							int iAttempts = 3;
							while (iAttempts > 0)
							{
								iRnd = rand() % 5;
								pThisStr[strlen(pThisStr) - 5] = 0;
								char pNewStr[MAX_PATH];
								sprintf(pNewStr, "%s%d.wav", pThisStr, 1 + iRnd);
								strcpy(pThisStr, pNewStr);
								if (FileExist(pThisStr) == 1)
								{
									// found variant, use this!
									if (iRnd != iOriginal)
										break;
								}
								else
								{
									// not exist
									pThisStr[strlen(pThisStr) - 5] = 0;
									sprintf(pNewStr, "%s%d.wav", pThisStr, 1 + iOriginal);
									strcpy(pThisStr, pNewStr);
									iRnd = iOriginal;
								}
								iAttempts--;
							}
						}
					}
				}

				// sounds or videos
				if ( t.entityelement[t.e].soundset == 0 ) 
				{
					t.tvideofile_s = pSoundSet0; entity_loadvideoid ( );
					if ( t.tvideoid == -999 )
					{
						t.entityelement[t.e].soundset = 0;
					}
					else
					{
						if ( t.tvideoid > 0 ) 
							t.entityelement[t.e].soundset=t.tvideoid*-1;
						else
							t.entityelement[t.e].soundset=loadinternalsoundcore(pSoundSet0,1);
					}
					if ( t.game.runasmultiplayer == 1 ) mp_refresh ( );
				}
				if (  t.entityelement[t.e].soundset1 == 0 ) 
				{
					t.tvideofile_s = pSoundSet1; entity_loadvideoid ( );
					if ( t.tvideoid == -999 )
					{
						t.entityelement[t.e].soundset1 = 0;
					}
					else
					{
						if (  t.tvideoid>0 ) 
							t.entityelement[t.e].soundset1=t.tvideoid*-1;
						else
							t.entityelement[t.e].soundset1=loadinternalsoundcore(pSoundSet1,1);
					}
					if ( t.game.runasmultiplayer == 1 ) mp_refresh ( );
				}
				if (  t.entityelement[t.e].soundset2 == 0 ) 
				{
					t.entityelement[t.e].soundset2 = loadinternalsoundcore(pSoundSet2,1);
					if ( t.game.runasmultiplayer == 1 ) mp_refresh ( );
				}
				if (t.entityelement[t.e].soundset3 == 0)
				{
					t.entityelement[t.e].soundset3 = loadinternalsoundcore(pSoundSet3, 1);
					if (t.game.runasmultiplayer == 1) mp_refresh ();
				}
				if (t.entityelement[t.e].soundset4 == 0)
				{
					t.entityelement[t.e].soundset4 = loadinternalsoundcore(pSoundSet4, 1);
					if (t.game.runasmultiplayer == 1) mp_refresh ();
				}
				if (t.entityelement[t.e].soundset5 == 0)
				{
					t.entityelement[t.e].soundset5 = loadinternalsoundcore(pSoundSet5, 1);
					if (t.game.runasmultiplayer == 1) mp_refresh ();
				}
				if (t.entityelement[t.e].soundset6 == 0)
				{
					t.entityelement[t.e].soundset6 = loadinternalsoundcore(pSoundSet6, 1);
					if (t.game.runasmultiplayer == 1) mp_refresh ();
				}

				// lipsync LIP data (associated with a sound file)
				if ( t.entityprofile[t.entid].ischaracter != 0 )
				{
					for (int s = 0; s <= 3; s++)
					{
						bool bFoundSound = false;
						if (s == 0 && t.entityelement[t.e].soundset != 0) bFoundSound = true;
						if (s == 1 && t.entityelement[t.e].soundset1 != 0) bFoundSound = true;
						if (s == 2 && t.entityelement[t.e].soundset2 != 0) bFoundSound = true;
						if (s == 3 && t.entityelement[t.e].soundset3 != 0) bFoundSound = true;
						if (bFoundSound == true)
						{
							// construct LIP filename for this sound
							char pWAVFile[2048];
							char pLIPFile[2048];
							if (s == 0) strcpy(pLIPFile, pSoundSet0);
							if (s == 1) strcpy(pLIPFile, pSoundSet1);
							if (s == 2) strcpy(pLIPFile, pSoundSet2);
							if (s == 3) strcpy(pLIPFile, pSoundSet3);
							strcpy(pWAVFile, pLIPFile);
							pLIPFile[strlen(pLIPFile) - 4] = 0;
							strcat(pLIPFile, ".lip");

							// and then get real location
							char pRealLIPFile[MAX_PATH];
							strcpy(pRealLIPFile, pLIPFile);
							GG_GetRealPath(pRealLIPFile, 0);

							// load in mouth shape data from LIP file
							if (s == 0) t.entityelement[t.e].lipset.clear();
							if (s == 1) t.entityelement[t.e].lipset1.clear();
							if (s == 2) t.entityelement[t.e].lipset2.clear();
							if (s == 3) t.entityelement[t.e].lipset3.clear();
							Dim(t.data_s, 9999);
							LoadArray(pRealLIPFile, t.data_s);
							for (t.l = 0; t.l <= 9999; t.l++)
							{
								t.line_s = t.data_s[t.l];
								LPSTR pLine = t.line_s.Get();
								if (Len(pLine) > 0)
								{
									char pTimeStr[32];
									char pMouthShapeStr[32];
									strcpy(pTimeStr, pLine);
									strcpy(pMouthShapeStr, "X");
									for (int n = 0; n < strlen(pTimeStr); n++)
									{
										if (pTimeStr[n] == 9)
										{
											pTimeStr[n] = 0;
											strcpy(pMouthShapeStr, pLine + n + 1);
											break;
										}
									}
									sCharacterCreatorPlusMouthData mouthDataShape;
									mouthDataShape.fTimeStamp = atof(pTimeStr);
									int iMouthShapeFrameIndex = 0;
									switch (pMouthShapeStr[0])
									{
									case 'A': iMouthShapeFrameIndex = 6;  break; // M
									case 'B': iMouthShapeFrameIndex = 2;  break; // K, S, T, EE
									case 'C': iMouthShapeFrameIndex = 1;  break; // AE 
									case 'D': iMouthShapeFrameIndex = 11; break; // AA
									case 'E': iMouthShapeFrameIndex = 7;  break; // AO
									case 'F': iMouthShapeFrameIndex = 9;  break; // W
									case 'G': iMouthShapeFrameIndex = 3;  break; // F V
									case 'H': iMouthShapeFrameIndex = 5;  break; // L
									case 'X': iMouthShapeFrameIndex = 0;  break; // CLOSED RELAXED MOUTH
									}
									mouthDataShape.iMouthShape = iMouthShapeFrameIndex;
									if (s == 0) t.entityelement[t.e].lipset.push_back(mouthDataShape);
									if (s == 1) t.entityelement[t.e].lipset1.push_back(mouthDataShape);
									if (s == 2) t.entityelement[t.e].lipset2.push_back(mouthDataShape);
									if (s == 3) t.entityelement[t.e].lipset3.push_back(mouthDataShape);
								}
							}
							UnDim(t.data_s);
						}
					}
				}
			}
		}
	}
}

void entity_cleargrideleprofrelationshipdata (void)
{
	// wipe out relational data when adding new object
	t.grideleprof.iObjectLinkID = 0;
	for (int i = 0; i < 10; i++)
	{
		t.grideleprof.iObjectRelationships[i] = 0;
		t.grideleprof.iObjectRelationshipsData[i] = 0;
		t.grideleprof.iObjectRelationshipsType[i] = 0;
	}
}

void entity_fillgrideleproffromprofile ( void )
{
	// Name
	t.grideleprof.name_s=t.entityprofileheader[t.entid].desc_s;

	t.grideleprof.blendmode = t.entityprofile[t.entid].blendmode;

	t.grideleprof.iFlattenID = -1; // never carries ID of individual elements
	if (!g_bEnableAutoFlattenSystem) //PE: If disabled always disable autoflatten.
		t.grideleprof.bAutoFlatten = false;

	// smart system to auto assign whether character is an ally or enemy
	t.grideleprof.iCharAlliance = 0;
	if ( strstr(t.grideleprof.name_s.Lower().Get(), "ally" ) != NULL )
	{
		t.grideleprof.iCharAlliance = 1;
	}

	// Group reference
	t.grideleprof.groupreference = t.entityprofile[t.entid].groupreference;

	// AI values
	t.grideleprof.aimain_s=t.entityprofile[t.entid].aimain_s;

	// AI use vars
	t.grideleprof.usekey_s=t.entityprofile[t.entid].usekey_s;
	t.grideleprof.ifused_s=t.entityprofile[t.entid].ifused_s;

	//  Spawn
	t.grideleprof.spawnatstart=t.entityprofile[t.entid].spawnatstart;
	t.grideleprof.spawnmax=t.entityprofile[t.entid].spawnmax;
	t.grideleprof.spawndelay=t.entityprofile[t.entid].spawndelay;
	t.grideleprof.spawnqty=t.entityprofile[t.entid].spawnqty;
	t.grideleprof.spawnupto=t.entityprofile[t.entid].spawnupto;
	t.grideleprof.spawnafterdelay=t.entityprofile[t.entid].spawnafterdelay;
	t.grideleprof.spawnwhendead=t.entityprofile[t.entid].spawnwhendead;
	t.grideleprof.spawndelayrandom=t.entityprofile[t.entid].spawndelayrandom;
	t.grideleprof.spawnqtyrandom=t.entityprofile[t.entid].spawnqtyrandom;
	t.grideleprof.spawnvel=t.entityprofile[t.entid].spawnvel;
	t.grideleprof.spawnvelrandom=t.entityprofile[t.entid].spawnvelrandom;
	t.grideleprof.spawnangle=t.entityprofile[t.entid].spawnangle;
	t.grideleprof.spawnanglerandom=t.entityprofile[t.entid].spawnanglerandom;
	t.grideleprof.spawnlife=t.entityprofile[t.entid].spawnlife;

	//  Scale, Cone
	t.grideleprof.scale=t.entityprofile[t.entid].scale;
	t.grideleprof.coneheight=t.entityprofile[t.entid].coneheight;
	t.grideleprof.coneangle=t.entityprofile[t.entid].coneangle;
	t.grideleprof.conerange=t.entityprofile[t.entid].conerange;

	//  Texture and Effect Data
	t.grideleprof.uniqueelement=0;
	t.grideleprof.texd_s=t.entityprofile[t.entid].texd_s;
	t.grideleprof.texaltd_s=t.entityprofile[t.entid].texaltd_s;
	t.grideleprof.effect_s=t.entityprofile[t.entid].effect_s;
	t.grideleprof.transparency=t.entityprofile[t.entid].transparency;
	t.grideleprof.castshadow=t.entityprofile[t.entid].castshadow;
	t.grideleprof.reducetexture=t.entityprofile[t.entid].reducetexture;

	//  Strength and Quantity
	t.grideleprof.strength=t.entityprofile[t.entid].strength;
	t.grideleprof.lives=t.entityprofile[t.entid].lives;
	t.grideleprof.isimmobile = t.entityprofile[t.entid].isimmobile;
	t.grideleprof.iscollectable = t.entityprofile[t.entid].iscollectable;
	t.grideleprof.lodmodifier=t.entityprofile[t.entid].lodmodifier;
	t.grideleprof.isocluder=t.entityprofile[t.entid].isocluder;
	t.grideleprof.isocludee=t.entityprofile[t.entid].isocludee;
	t.grideleprof.lootpercentage=t.entityprofile[t.entid].lootpercentage;
	t.grideleprof.colondeath=t.entityprofile[t.entid].colondeath;
	t.grideleprof.parententityindex=t.entityprofile[t.entid].parententityindex;
	t.grideleprof.parentlimbindex=t.entityprofile[t.entid].parentlimbindex;
	t.grideleprof.isviolent=t.entityprofile[t.entid].isviolent;
	t.grideleprof.cantakeweapon=t.entityprofile[t.entid].cantakeweapon;
	t.grideleprof.hasweapon_s=t.entityprofile[t.entid].hasweapon_s;
	t.grideleprof.quantity=t.entityprofile[t.entid].quantity;
	t.grideleprof.isobjective = t.entityprofile[t.entid].isobjective;
	t.grideleprof.hurtfall=t.entityprofile[t.entid].hurtfall;
	t.grideleprof.speed=t.entityprofile[t.entid].speed;
	t.grideleprof.animspeed=t.entityprofile[t.entid].animspeed;

	//  Decal and Sound Name
	t.grideleprof.voiceset_s=t.entityprofile[t.entid].voiceset_s;
	t.grideleprof.voicerate=t.entityprofile[t.entid].voicerate;
	t.grideleprof.soundset_s=t.entityprofile[t.entid].soundset_s;
	t.grideleprof.soundset1_s=t.entityprofile[t.entid].soundset1_s;
	t.grideleprof.soundset2_s=t.entityprofile[t.entid].soundset2_s;
	t.grideleprof.soundset3_s=t.entityprofile[t.entid].soundset3_s;
	t.grideleprof.soundset4_s=t.entityprofile[t.entid].soundset4_s;
	t.grideleprof.soundset5_s=t.entityprofile[t.entid].soundset5_s;
	t.grideleprof.soundset6_s=t.entityprofile[t.entid].soundset6_s;
	t.grideleprof.soundset4a_s = t.entityprofile[t.entid].soundset4a_s;
	t.grideleprof.overrideanimset_s = "";

	//  FPGC - 310710 - decal particle settings
	t.particlefile_s = ""; //t.grideleprof.basedecal_s; //PE: Not used anymore.
	decal_getparticlefile ( );
	t.grideleprof.particleoverride=1;
	t.grideleprof.particle=g.gotparticle;

	//  Marker Data
	t.grideleprof.markerindex=t.entityprofile[t.entid].markerindex;
	t.grideleprof.light=t.entityprofile[t.entid].light;
	t.grideleprof.trigger=t.entityprofile[t.entid].trigger;
	t.grideleprof.usespotlighting=t.entityprofile[t.entid].usespotlighting;

	//  Data Extracted From GUN and FLAK
	t.tgunid_s=t.entityprofile[t.entid].isweapon_s;
	entity_getgunidandflakid ( );
	t.grideleprof.rateoffire=t.entityprofile[t.entid].rateoffire;
	t.grideleprof.weaponisammo=0;
	if (  t.tgunid>0 ) 
	{
		t.grideleprof.accuracy=g.firemodes[t.tgunid][0].settings.accuracy;
		t.grideleprof.reloadqty=g.firemodes[t.tgunid][0].settings.reloadqty;
		t.grideleprof.fireiterations=g.firemodes[t.tgunid][0].settings.iterate;
		t.grideleprof.usespotlighting=g.firemodes[t.tgunid][0].settings.usespotlighting;
		if (  t.tflakid == 0 ) 
		{
			t.grideleprof.damage=g.firemodes[t.tgunid][0].settings.damage;
			t.grideleprof.range=g.firemodes[t.tgunid][0].settings.range;
			t.grideleprof.dropoff = g.firemodes[t.tgunid][0].settings.dropoff;
			t.grideleprof.clipcapacity = g.firemodes[t.tgunid][0].settings.clipcapacity;
			t.grideleprof.weaponpropres1 = g.firemodes[t.tgunid][0].settings.weaponpropres1;
			t.grideleprof.weaponpropres2 = g.firemodes[t.tgunid][0].settings.weaponpropres2;
		}
		else
		{
			t.grideleprof.damage=0;
			t.grideleprof.lifespan=0;
			t.grideleprof.throwspeed=0;
			t.grideleprof.throwangle=0;
			t.grideleprof.bounceqty=0;
			t.grideleprof.explodeonhit=0;
			t.grideleprof.weaponisammo=t.tflakid;
		}
	}

	// Collision Data Overide
	t.grideleprof.iOverrideCollisionMode = -1;

	// Physics Data
	t.grideleprof.physics=t.entityprofile[t.entid].physics;
	t.grideleprof.phyalways=t.entityprofile[t.entid].phyalways;
	t.grideleprof.phyweight=t.entityprofile[t.entid].phyweight;
	t.grideleprof.phyfriction=t.entityprofile[t.entid].phyfriction;
	t.grideleprof.phyforcedamage=t.entityprofile[t.entid].phyforcedamage;
	t.grideleprof.rotatethrow=t.entityprofile[t.entid].rotatethrow;
	t.grideleprof.explodable=t.entityprofile[t.entid].explodable;
	t.grideleprof.explodedamage=t.entityprofile[t.entid].explodedamage;
	t.grideleprof.explodeheight =t.entityprofile[t.entid].explodeheight;
	t.grideleprof.explodable_decalname = t.entityprofile[t.entid].explodable_decalname;
	
	//t.grideleprof.iMaterialSoundIndex
	
	// 301115 - data extracted from neighbors (LOD Modifiers are shared across all parent copies)
	int iThisBankIndex = t.entid;
	if ( t.entityprofile[iThisBankIndex].addhandlelimb==0 )
	{
		for ( int e=1; e<=g.entityelementlist; e++ )
		{
			if ( t.entityelement[e].bankindex==iThisBankIndex )
			{
				t.grideleprof.lodmodifier = t.entityelement[e].eleprof.lodmodifier;
				break;
			}
		}
	}

	//PE: Make sure when we create we use default variables in eleprof.
	// Users don't want to have to enable custom materials before editing materials
	// ...non-custom materials are also causing issues with emissive, so just enable by default
	// LB: Preben, OLDFLAK argues that users WANT this off by default and was the previous default behavior
	// so we will make this false for default and fix any new issues that may arise, including the new demand
	// that other users want if ON by default :)  Perhaps something in editor pref settings ;)
	// LB: Additional - Preben notes doing the above breaks many things, including FPE material settings, so restore and rethink
	// PE: Keep it as is , and now use bUseFPESettings to control what will get updated.
	t.grideleprof.bCustomWickedMaterialActive = true;
	t.grideleprof.WEMaterial = t.entityprofile[t.entid].WEMaterial;

	//Need default particle setup here. or if will use the last inside "t.grideleprof".
	t.grideleprof.newparticle.emitterid = -1;
	t.grideleprof.newparticle.emittername = "particlesbank/default";

	// wipe out relational data when adding new object
	entity_cleargrideleprofrelationshipdata();

	// when first load an object, need to populate sound4 so _properties is ALWAYS called!
	cstr script_name = "scriptbank\\";
	script_name += t.grideleprof.aimain_s;
	extern void ParseLuaScript(entityeleproftype *tmpeleprof, char * script);
	ParseLuaScript(&t.grideleprof, script_name.Get());
}

void entity_updatetextureandeffectfromeleprof ( void )
{

	//  Texture and Effect (use entityprofile loader)
	t.storeentdefaults=t.entityprofile[t.entid];
	t.entityprofile[t.entid].texd_s=t.entityelement[t.e].eleprof.texd_s;
	t.entityprofile[t.entid].texaltd_s=t.entityelement[t.e].eleprof.texaltd_s;
	t.entityprofile[t.entid].texdid=t.entityelement[t.e].eleprof.texdid;
	t.entityprofile[t.entid].texaltdid=t.entityelement[t.e].eleprof.texaltdid;
	t.entityprofile[t.entid].effect_s=t.entityelement[t.e].eleprof.effect_s;
	t.entityprofile[t.entid].iscollectable =t.entityelement[t.e].eleprof.iscollectable;
	t.entityprofile[t.entid].texnid=t.entityelement[t.e].eleprof.texnid;
	t.entityprofile[t.entid].texsid=t.entityelement[t.e].eleprof.texsid;
	t.entityprofile[t.entid].texidmax=t.entityelement[t.e].eleprof.texidmax;
	t.entityprofile[t.entid].transparency=t.entityelement[t.e].eleprof.transparency;
	t.entityprofile[t.entid].reducetexture=t.entityelement[t.e].eleprof.reducetexture;
	entity_loadtexturesandeffect ( );
	t.entityelement[t.e].eleprof.texd_s=t.entityprofile[t.entid].texd_s;
	t.entityelement[t.e].eleprof.texaltd_s=t.entityprofile[t.entid].texaltd_s;
	t.entityelement[t.e].eleprof.texdid=t.entityprofile[t.entid].texdid;
	t.entityelement[t.e].eleprof.texaltdid=t.entityprofile[t.entid].texaltdid;
	t.entityelement[t.e].eleprof.effect_s=t.entityprofile[t.entid].effect_s;
	t.entityelement[t.e].eleprof.iscollectable =t.entityprofile[t.entid].iscollectable;
	t.entityelement[t.e].eleprof.texnid=t.entityprofile[t.entid].texnid;
	t.entityelement[t.e].eleprof.texsid=t.entityprofile[t.entid].texsid;
	t.entityelement[t.e].eleprof.texidmax=t.entityprofile[t.entid].texidmax;
	t.entityelement[t.e].eleprof.transparency=t.entityprofile[t.entid].transparency;
	t.entityelement[t.e].eleprof.reducetexture=t.entityprofile[t.entid].reducetexture;
	t.entityprofile[t.entid]=t.storeentdefaults;
}

void entity_updatetextureandeffectfromgrideleprof ( void )
{
	//  Texture and Effect (use entityprofile loader)
	//t.storeentdefaults as entityprofiletype;
	t.storeentdefaults=t.entityprofile[t.entid];
	t.entityprofile[t.entid].texd_s=t.grideleprof.texd_s;
	t.entityprofile[t.entid].texaltd_s=t.grideleprof.texaltd_s;
	t.entityprofile[t.entid].texdid=t.grideleprof.texdid;
	t.entityprofile[t.entid].texaltdid=t.grideleprof.texaltdid;
	t.entityprofile[t.entid].effect_s=t.grideleprof.effect_s;
	t.entityprofile[t.entid].iscollectable =t.grideleprof.iscollectable;
	t.entityprofile[t.entid].texnid=t.grideleprof.texnid;
	t.entityprofile[t.entid].texsid=t.grideleprof.texsid;
	t.entityprofile[t.entid].texidmax=t.grideleprof.texidmax;
	t.entityprofile[t.entid].transparency=t.grideleprof.transparency;
	t.entityprofile[t.entid].reducetexture=t.grideleprof.reducetexture;
	entity_loadtexturesandeffect ( );
	t.grideleprof.texd_s=t.entityprofile[t.entid].texd_s;
	t.grideleprof.texaltd_s=t.entityprofile[t.entid].texaltd_s;
	t.grideleprof.texdid=t.entityprofile[t.entid].texdid;
	t.grideleprof.texaltdid=t.entityprofile[t.entid].texaltdid;
	t.grideleprof.effect_s=t.entityprofile[t.entid].effect_s;
	t.grideleprof.iscollectable =t.entityprofile[t.entid].iscollectable;
	t.grideleprof.texnid=t.entityprofile[t.entid].texnid;
	t.grideleprof.texsid=t.entityprofile[t.entid].texsid;
	t.grideleprof.texidmax=t.entityprofile[t.entid].texidmax;
	t.grideleprof.transparency=t.entityprofile[t.entid].transparency;
	t.grideleprof.reducetexture=t.entityprofile[t.entid].reducetexture;
	t.entityprofile[t.entid]=t.storeentdefaults;
}

void entity_getgunidandflakid ( void )
{
	//  Use Weapon Name to get GUNID and FLAKID
	if (  t.tgunid_s != "" ) 
	{
		//  get gun
		t.findgun_s=Lower(t.tgunid_s.Get());
		gun_findweaponindexbyname ( );
		t.tgunid=t.foundgunid;
		//  no flak - old system
		t.tflakid=0;
	}
	else
	{
		t.tgunid=0 ; t.tflakid=0;
	}
}

void entity_loadtexturesandeffect ( void )
{
	//  If entity object not exist, reset var
	if (  t.entobj>0 ) 
	{
		if (  ObjectExist(t.entobj) == 0  )  t.entobj = 0;
	}

	//  Only characters need a higher quality texture, rest use divide standard settings
	t.tfullorhalfdivide=0;
	if (  t.segobjusedformapeditor == 0 ) 
	{
		if (  t.entityprofile[t.entid].ischaracter == 1 ) 
		{
			t.tfullorhalfdivide=2;
		}
		else
		{
			if (  t.entityprofile[t.entid].reducetexture != 0 ) 
			{
				if (  t.entityprofile[t.entid].reducetexture == -1 ) 
				{
					t.tfullorhalfdivide=1;
				}
				else
				{
					t.tfullorhalfdivide=2;
				}
			}
		}
	}

	//  Apply TEXTURE to entity object
	bool bMultiMaterialObject = false;
	t.tuseeffecttexture=0;
	t.texdir_s = "";
	t.texaltdir_s = "";
	t.tfile_s=t.entityprofile[t.entid].texd_s;
	t.tfilealt_s=t.entityprofile[t.entid].texaltd_s;
	if (t.tfile_s != "")
	{
		if (t.entityprofile[t.entid].texpath_s != "")
		{
			t.texdir_s = t.entityprofile[t.entid].texpath_s + t.tfile_s;
			t.texaltdir_s = t.entityprofile[t.entid].texpath_s + t.tfilealt_s;
		}
		else
		{
			// wicked relies on accurate texture path and name entries, and a Classic bug caused an error
			// when a full texture path was already provided, i.e. gamecore\guns\modern, etc
			bool bPathInsideFilename = false;
			LPSTR pFilenamePart = t.tfile_s.Get();
			for (int n = 0; n < strlen(pFilenamePart); n++)
				if (pFilenamePart[n] == '\\' || pFilenamePart[n] == '/')
					bPathInsideFilename = true;
			if ( bPathInsideFilename == true )
			{
				// path is already provided in t.tfile_s!
				t.texdir_s = t.tfile_s;
				t.texaltdir_s = t.tfilealt_s;
			}
			else
			{
				// regular assembly of entity folder, entity path and entity filename only
				t.texdir_s = t.entdir_s + t.entpath_s + t.tfile_s;
				t.texaltdir_s = t.entdir_s + t.entpath_s + t.tfilealt_s;
			}
		}
	}
	// Wicked does not support old method of loading images/effects
	t.entityprofile[t.entid].texaltdid=0;

	// Texture and apply effect
	// remove '.dds' from texture filename
	char pNoExtFilename[1024];
	strcpy ( pNoExtFilename, t.texdir_s.Get() );
	if (strlen(pNoExtFilename) > 4)
	{
		// strip .DDS/.JPG/etc from filename
		pNoExtFilename[strlen(pNoExtFilename) - 4] = 0;

		// for valid objects
		sObject* pObject = GetObjectData(t.entobj);
		if (pObject)
		{
			// Leave texture filename alone with .DDS/.PNG intact (not a WickedPBR texture set designation)
			t.texdirnoext_s = t.texdir_s;

			// go through all meshes in object and texture them
			bool bApplyTexture = false;
			for (int iMeshIndex = 0; iMeshIndex < pObject->iMeshCount; iMeshIndex++)
			{
				sMesh* pMesh = pObject->ppMeshList[iMeshIndex];
				if (pMesh)
				{
					if (pMesh->dwTextureCount > 0)
					{
						// if not enough textures create required number
						if (pMesh->dwTextureCount < GG_MESH_TEXTURE_SURFACE)
						{
							extern bool EnsureTextureStageValid (sMesh* pMesh, int iTextureStage);
							EnsureTextureStageValid (pMesh, GG_MESH_TEXTURE_SURFACE);
						}

						// determine if base texture is a _color, in which case we can organize the texture array properly
						if (strnicmp (pNoExtFilename + strlen(pNoExtFilename) - 6, "_color", 6) == NULL)
						{
							// strip _color from pNoExtFilename
							pNoExtFilename[strlen(pNoExtFilename) - 6] = 0;

							// construct cousin texture references
							if (pMesh->dwTextureCount >= GG_MESH_TEXTURE_SURFACE)
							{
								strcpy(pMesh->pTextures[GG_MESH_TEXTURE_DIFFUSE].pName, pNoExtFilename);
								strcat(pMesh->pTextures[GG_MESH_TEXTURE_DIFFUSE].pName, "_color.dds");
								strcpy(pMesh->pTextures[GG_MESH_TEXTURE_NORMAL].pName, pNoExtFilename);
								strcat(pMesh->pTextures[GG_MESH_TEXTURE_NORMAL].pName, "_normal.dds");
								strcpy(pMesh->pTextures[GG_MESH_TEXTURE_ROUGHNESS].pName, pNoExtFilename);
								strcat(pMesh->pTextures[GG_MESH_TEXTURE_ROUGHNESS].pName, "_surface.dds");
								pMesh->pTextures[GG_MESH_TEXTURE_ROUGHNESS].channelMask = (15) + (1 << 4);
								strcpy(pMesh->pTextures[GG_MESH_TEXTURE_METALNESS].pName, pNoExtFilename);
								strcat(pMesh->pTextures[GG_MESH_TEXTURE_METALNESS].pName, "_surface.dds");
								pMesh->pTextures[GG_MESH_TEXTURE_METALNESS].channelMask = (15) + (2 << 4);
								strcpy(pMesh->pTextures[GG_MESH_TEXTURE_OCCLUSION].pName, pNoExtFilename);
								strcat(pMesh->pTextures[GG_MESH_TEXTURE_OCCLUSION].pName, "_surface.dds");
								pMesh->pTextures[GG_MESH_TEXTURE_OCCLUSION].channelMask = (15) + (0 << 4);
								strcpy(pMesh->pTextures[GG_MESH_TEXTURE_EMISSIVE].pName, pNoExtFilename);
								strcat(pMesh->pTextures[GG_MESH_TEXTURE_EMISSIVE].pName, "_emissive.dds");
								strcpy(pMesh->pTextures[GG_MESH_TEXTURE_SURFACE].pName, pNoExtFilename);
								strcat(pMesh->pTextures[GG_MESH_TEXTURE_SURFACE].pName, "_surface.dds");
							}
						}
						else
						{
							// simple base texture
							strcpy(pMesh->pTextures[0].pName, t.texdirnoext_s.Get());
						}
						bApplyTexture = true;
					}
				}
			}
			if (bApplyTexture == true)
			{
				WickedSetEntityId(t.entid);
				WickedCall_TextureObject(pObject, NULL);
				WickedSetEntityId(-1);
			}
		}
	}

	// Set any entity transparenct
	if ( t.entobj>0 ) 
	{
		if (t.entityprofile[t.entid].transparency >= 0)
		{
			WickedSetEntityId(t.entid);
			SetObjectTransparency(t.entobj, t.entityprofile[t.entid].transparency);
			WickedSetEntityId(-1);
		}
	}

	// Set entity culling (added COLLMODE 300114)
	if (t.entityprofile[t.entid].cullmode >= 0)
	{
		// For Wicked, cull mode controlled per-mesh with parent default as normal 
		
		//PE: Prefer WEMaterial over old cullmode
		bool bUseWEMaterial = false;
		if (t.entityprofile[t.entid].WEMaterial.MaterialActive)
		{
			WickedSetEntityId(t.entid);
			WickedSetElementId(0);
			sObject* pObject = g_ObjectList[t.entobj];
			if (pObject)
			{
				bUseWEMaterial = true;
				for (int iMeshIndex = 0; iMeshIndex < pObject->iMeshCount; iMeshIndex++)
				{
					sMesh* pMesh = pObject->ppMeshList[iMeshIndex];
					if (pMesh)
					{
						// set properties of mesh
						WickedSetMeshNumber(iMeshIndex);
						bool bDoubleSided = WickedDoubleSided();
						if (bDoubleSided)
						{
							pMesh->bCull = false;
							pMesh->iCullMode = 0;
							WickedCall_SetMeshCullmode(pMesh);
						}
						else
						{
							pMesh->iCullMode = 1;
							pMesh->bCull = true;
							WickedCall_SetMeshCullmode(pMesh);
						}
					}
				}
			}
			WickedSetEntityId(-1);
		}

		if(!bUseWEMaterial)
		{
			SetObjectCull(t.entobj, 1);
		}
	}

	// Set cull mode for limbs (if hair specified)
	if ( t.entityprofile[t.entid].hairframestart != -1 )
	{
		for ( int tlmb = t.entityprofile[t.entid].hairframestart; tlmb <= t.entityprofile[t.entid].hairframefinish; tlmb++ )
		{
			if ( LimbExist ( t.entobj, tlmb ) == 1 )
			{
				SetLimbCull ( t.entobj, tlmb, 0 );
			}
		}
	}

	// hide specified limbs
	if ( t.entityprofile[t.entid].hideframestart != -1 )
	{
		for ( int tlmb = t.entityprofile[t.entid].hideframestart; tlmb <= t.entityprofile[t.entid].hideframefinish; tlmb++ )
		{
			if ( LimbExist ( t.entobj, tlmb ) == 1 )
			{
				ExcludeLimbOn ( t.entobj, tlmb );
			}
		}
	}

	//PE: Below will enable additive i wicked.

}

//PE: Faster loading
#define USEFASTLOADING

#ifdef USEFASTLOADING
char * c_data = NULL;
char * c_data_pointer = NULL;
int c_data_size = 0;
HANDLE c_hFile;
extern DBPRO_GLOBAL char m_pWorkString[_MAX_PATH];
DARKSDK LPSTR GetReturnStringFromWorkString(char* WorkString = m_pWorkString);

void c_OpenToRead(int f, LPSTR pFilename)
{
	//Read everything.

	HANDLE hreadfile = GG_CreateFile(pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hreadfile != INVALID_HANDLE_VALUE)
	{
		int filebuffersize = GetFileSize(hreadfile, NULL);
		c_data = new char[filebuffersize+256];
		// Read file into memory
		DWORD bytesread;
		ReadFile(hreadfile, c_data, filebuffersize, &bytesread, NULL);
		CloseHandle(hreadfile);
		c_data_pointer = c_data;
		c_data_size = filebuffersize;
	}
	else
	{
		c_data = NULL;
		c_data_pointer = c_data;
		c_data_size = 0;
	}

}
//delete(c_data);

bool c_ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped)
{
	if ( ((c_data_pointer-c_data) + nNumberOfBytesToRead) > c_data_size)
		return false;
	memcpy(lpBuffer, c_data_pointer, nNumberOfBytesToRead);
	c_data_pointer += nNumberOfBytesToRead;
	*lpNumberOfBytesRead = nNumberOfBytesToRead;
	return true;
}

int c_ReadLong(int f)
{
	if (!c_data) return 0;
	int iResult = 0;
	DWORD bytes;
	// Read from file
	DWORD data;
	if (c_ReadFile(c_hFile, &data, sizeof(data), &bytes, NULL) == 0)
		return(0); // RunTimeWarning(RUNTIMEERROR_CANNOTREADFROMFILE);
	iResult = data;
	return iResult;
}


void c_CloseFile(int f)
{
	if(c_data) delete(c_data);
	c_data = NULL;
	c_data_pointer = c_data;
	c_data_size = 0;
}

int c_ReadByte(int f)
{
	if (!c_data) return 0;

	int iResult = 0;
	DWORD bytes;
	// Read from file
	unsigned char data;
	if (c_ReadFile(c_hFile, &data, sizeof(data), &bytes, NULL) == 0)
		return(0); //RunTimeWarning(RUNTIMEERROR_CANNOTREADFROMFILE);

	iResult = data;
	return iResult;
}

int c_ReadWord(int f)
{
	if (!c_data) return 0;

	int iResult = 0;
	DWORD bytes;
	// Read from file
	WORD data;
			
	if (c_ReadFile(c_hFile, &data, sizeof(data), &bytes, NULL) == 0)
		return(0); //RunTimeWarning(RUNTIMEERROR_CANNOTREADFROMFILE);

	iResult = data;
	return iResult;
}


float c_ReadFloat(int f)
{
	if (!c_data) return 0;

	float fResult = 0.0f;
	DWORD bytes;
	// Read from file
	float data;
	if (c_ReadFile(c_hFile, &data, sizeof(data), &bytes, NULL) == 0)
		return(0); //RunTimeWarning(RUNTIMEERROR_CANNOTREADFROMFILE);

	fResult = data;

	return fResult;
}

char c_filerror[2] = "";

//PE: We use cstr t.a_s = c_ReadString ( 1 ) , so mem is never freed.
char c_ReadStringRing[20][1024];
int c_ReadStringCount = 0;
LPSTR c_ReadString(int f)
{
	if (!c_data) return 0;

	c_ReadStringCount = c_ReadStringCount + 1;
	if (c_ReadStringCount > 19)
		c_ReadStringCount = 0;
	
	LPSTR pReturnString = 0;

	unsigned char c = 0;
	DWORD bytes;
	std::vector<char> WorkString;

	bool eof = false;
	do
	{
		if (c_ReadFile(c_hFile, &c, 1, &bytes, NULL) == 0)
		{
			//RunTimeWarning(RUNTIMEERROR_CANNOTREADFROMFILE);
			return(&c_filerror[0]);
		}
		if (bytes == 0)
		{
			eof = true;
		}
		else if (c >= 32 || c == 9)
		{
			WorkString.push_back(c);
		}
	} while ((c >= 32 || c == 9) && !eof);

	WorkString.push_back(0);

	if (c == 13)
	{
		if (c_ReadFile(c_hFile, &c, 1, &bytes, NULL) == 0)
		{
			//RunTimeWarning(RUNTIMEERROR_CANNOTREADFROMFILE);
			return(&c_filerror[0]);
		}
	}

	// Create and return string
	if (WorkString.size() < 1024)
	{
		strcpy(&c_ReadStringRing[c_ReadStringCount][0], &WorkString[0]);
		pReturnString = &c_ReadStringRing[c_ReadStringCount][0];
	}
	else
		pReturnString = GetReturnStringFromWorkString(&WorkString[0]);

	return pReturnString;
}

LPSTR c_ReadStringOLD(int f)
{
	if (!c_data) return 0;

	LPSTR pReturnString = 0;

	unsigned char c = 0;
	DWORD bytes;
	std::vector<char> WorkString;

	bool eof = false;
	do
	{
		if (c_ReadFile(c_hFile, &c, 1, &bytes, NULL) == 0)
		{
			//RunTimeWarning(RUNTIMEERROR_CANNOTREADFROMFILE);
			return(&c_filerror[0]);
		}
		if (bytes == 0)
		{
			eof = true;
		}
		else if (c >= 32 || c == 9)
		{
			WorkString.push_back(c);
		}
	} while ((c >= 32 || c == 9) && !eof);

	WorkString.push_back(0);

	if (c == 13)
	{
		if (c_ReadFile(c_hFile, &c, 1, &bytes, NULL) == 0)
		{
			//RunTimeWarning(RUNTIMEERROR_CANNOTREADFROMFILE);
			return(&c_filerror[0]);
		}
	}

	// Create and return string
	pReturnString = GetReturnStringFromWorkString(&WorkString[0]);

	return pReturnString;
}

//PE: We can have 0x0a in soundset4 (entered text) so always use 13 to stop.
LPSTR c_ReadStringIncl0xA(int f)
{
	if (!c_data) return 0;

	c_ReadStringCount = c_ReadStringCount + 1;
	if (c_ReadStringCount > 19)
		c_ReadStringCount = 0;

	LPSTR pReturnString = 0;

	unsigned char c = 0;
	DWORD bytes;
	std::vector<char> WorkString;

	bool eof = false;
	do
	{
		if (c_ReadFile(c_hFile, &c, 1, &bytes, NULL) == 0)
		{
			//RunTimeWarning(RUNTIMEERROR_CANNOTREADFROMFILE);
			return(&c_filerror[0]);
		}
		if (bytes == 0)
		{
			eof = true;
		}
		else if (c >= 32 || c == 9 || c == 10)
		{
			WorkString.push_back(c);
		}
	} while ((c >= 32 || c == 9 || c == 10) && !eof);

	WorkString.push_back(0);

	if (c == 13)
	{
		if (c_ReadFile(c_hFile, &c, 1, &bytes, NULL) == 0)
		{
			//RunTimeWarning(RUNTIMEERROR_CANNOTREADFROMFILE);
			return(&c_filerror[0]);
		}
	}

	// Create and return string
	if (WorkString.size() < 1024)
	{
		strcpy(&c_ReadStringRing[c_ReadStringCount][0], &WorkString[0]);
		pReturnString = &c_ReadStringRing[c_ReadStringCount][0];
	}
	else
		pReturnString = GetReturnStringFromWorkString(&WorkString[0]);
	return pReturnString;
}

LPSTR c_ReadStringIncl0xAOLD(int f)
{
	if (!c_data) return 0;


	LPSTR pReturnString = 0;

	unsigned char c = 0;
	DWORD bytes;
	std::vector<char> WorkString;

	bool eof = false;
	do
	{
		if (c_ReadFile(c_hFile, &c, 1, &bytes, NULL) == 0)
		{
			//RunTimeWarning(RUNTIMEERROR_CANNOTREADFROMFILE);
			return(&c_filerror[0]);
		}
		if (bytes == 0)
		{
			eof = true;
		}
		else if (c >= 32 || c == 9 || c == 10)
		{
			WorkString.push_back(c);
		}
	} while ((c >= 32 || c == 9 || c == 10) && !eof);

	WorkString.push_back(0);

	if (c == 13)
	{
		if (c_ReadFile(c_hFile, &c, 1, &bytes, NULL) == 0)
		{
			//RunTimeWarning(RUNTIMEERROR_CANNOTREADFROMFILE);
			return(&c_filerror[0]);
		}
	}

	// Create and return string
	pReturnString = GetReturnStringFromWorkString(&WorkString[0]);
	return pReturnString;
}

int g_iAddEntityElementsMode = 0;

void c_entity_loadelementsdata ( void )
{
	// Free any old elements
	if (g_iAddEntityElementsMode == 0)
	{
		entity_deleteelementsdata ();
		if (t.elementsfilename_s == "")  t.elementsfilename_s = g.mysystem.levelBankTestMap_s + "map.ele";
	}

	// load entity element list
	t.failedtoload=0;
	t.versionnumbersupported = 342;

	if ( FileExist(t.elementsfilename_s.Get()) == 1 ) 
	{
		int iElementsInFile = 0;
		c_OpenToRead(1, t.elementsfilename_s.Get());
		t.versionnumberload = c_ReadLong ( 1 );
		if (  t.versionnumberload<100 ) 
		{
			//  Pre-version data - development only
			g.entityelementlist=t.versionnumberload;
			t.versionnumberload=100;
		}
		else
		{
			iElementsInFile = c_ReadLong (1);
			if (g_iAddEntityElementsMode == 0)
			{
				g.entityelementlist = iElementsInFile;
			}
			if (g_iAddEntityElementsMode == 1)
			{
				//g.entityelementlist += iElementsInFile;
			}
		}
		if ( t.versionnumberload <= t.versionnumbersupported ) 
		{
			if (iElementsInFile > 0)//g.entityelementlist>0 )
			{
				bool bFirstTimeOnlyToGrabGroupDataOops = true;
				if (g_iAddEntityElementsMode == 0)
				{
					UnDim (t.entityelement);
					UnDim2 (t.entityshadervar);
					UnDim (t.entitydebug_s);
					g.entityelementmax = g.entityelementlist;
					Dim (t.entityelement, g.entityelementmax);
					Dim2(t.entityshadervar, g.entityelementmax, g.globalselectedshadermax);
					Dim (t.entitydebug_s, g.entityelementmax);
				}
				if (g_iAddEntityElementsMode == 1 || g_iAddEntityElementsMode == 2)
				{
					// ensure there are enough free slots
					int iCount = 0;
					for (int finde = 1; finde <= g.entityelementlist; finde++)
					{
						if (t.entityelement[finde].bankindex == 0)
						{
							iCount++;
						}
					}
					if (iCount < iElementsInFile)
					{
						// make more space
						Dim (t.storeentityelement, g.entityelementmax);
						for (t.e = 1; t.e <= g.entityelementmax; t.e++)
						{
							t.storeentityelement[t.e] = t.entityelement[t.e];
						}
						UnDim (t.entityelement);
						UnDim (t.entityshadervar);
						int iOldSizeCount = g.entityelementmax;
						g.entityelementmax += iElementsInFile + 10;
						Dim (t.entityelement, g.entityelementmax);
						Dim2(t.entityshadervar, g.entityelementmax, g.globalselectedshadermax);
						for (t.e = 1; t.e <= iOldSizeCount; t.e++)
						{
							t.entityelement[t.e] = t.storeentityelement[t.e];
						}
					}
				}
				for ( int n = 1; n <= iElementsInFile; n++ )
				{
					bool bIncreasedListSize = false;
					if (g_iAddEntityElementsMode == 0)
					{
						t.e = n;
					}
					if (g_iAddEntityElementsMode == 1 || g_iAddEntityElementsMode == 2)
					{
						// find free slot in add mode
						bool bFoundFreeSlot = false;
						for (int finde = 1; finde <= g.entityelementlist; finde++)
						{
							if (t.entityelement[finde].bankindex == 0)
							{
								bFoundFreeSlot = true;
								t.e = finde;
								break;
							}
						}
						if (bFoundFreeSlot == false )
						{
							// increase max element list
							if (g.entityelementlist < g.entityelementmax - 1)
							{
								g.entityelementlist++;
								t.e = g.entityelementlist;
								bIncreasedListSize = true;
							}
							else
							{
								// this should never happen (see code above), but if so, just overwrite last slot
								t.e = g.entityelementmax - 1;
							}
						}

						// special flag so can handle entity with collection list later
						if(g_iAddEntityElementsMode == 1)
							t.entityelement[t.e].specialentityloadflag = 123;
					}
					if ( t.game.runasmultiplayer == 1 ) mp_refresh ( );
					//  actual file data
					if (  t.versionnumberload >= 101 ) 
					{
						//  Version 1.01
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].maintype=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].bankindex=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].staticflag=t.a;
						t.a_f = c_ReadFloat ( 1 ); t.entityelement[t.e].x=t.a_f;
						t.a_f = c_ReadFloat ( 1 ); t.entityelement[t.e].y=t.a_f;
						t.a_f = c_ReadFloat ( 1 ); t.entityelement[t.e].z=t.a_f;
						t.a_f = c_ReadFloat ( 1 ); t.entityelement[t.e].rx=t.a_f;
						t.a_f = c_ReadFloat ( 1 ); t.entityelement[t.e].ry=t.a_f;
						t.a_f = c_ReadFloat ( 1 ); t.entityelement[t.e].rz=t.a_f;
						t.a_s = c_ReadString ( 1 ); t.entityelement[t.e].eleprof.name_s=t.a_s;
						t.a_s = c_ReadString ( 1 ); // t.entityelement[t.e].eleprof.aiinit_s=t.a_s; //PE: Not used anymore.
						t.a_s = c_ReadString (1);
						if (strnicmp (t.a_s.Get(), "default.lua", 11) == NULL)
						{
							t.entityelement[t.e].eleprof.aimain_s = "no_behavior_selected.lua";
						}
						else
						{
							if (strlen(t.a_s.Get()) < 4)
							{
								t.entityelement[t.e].eleprof.aimain_s = "no_behavior_selected.lua";
							}
							else
							{
								t.entityelement[t.e].eleprof.aimain_s = t.a_s;
							}
						}
						t.a_s = c_ReadString ( 1 ); // t.entityelement[t.e].eleprof.aidestroy_s=t.a_s;  //PE: Not used anymore.
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.isobjective=t.a;
						t.a_s = c_ReadString ( 1 ); t.entityelement[t.e].eleprof.usekey_s=t.a_s;
						t.a_s = c_ReadString ( 1 ); t.entityelement[t.e].eleprof.ifused_s=t.a_s;
						t.a_s = c_ReadString ( 1 ); //t.entityelement[t.e].eleprof.ifusednear_s=t.a_s;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.uniqueelement=t.a;
						t.a_s = c_ReadString ( 1 ); t.entityelement[t.e].eleprof.texd_s=t.a_s;
						t.a_s = c_ReadString ( 1 ); t.entityelement[t.e].eleprof.texaltd_s=t.a_s;
						t.a_s = c_ReadString ( 1 ); t.entityelement[t.e].eleprof.effect_s=t.a_s;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.transparency=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].editorfixed=t.a;
						t.a_s = c_ReadString ( 1 ); t.entityelement[t.e].eleprof.soundset_s=t.a_s;
						t.a_s = c_ReadString ( 1 ); t.entityelement[t.e].eleprof.soundset1_s=t.a_s;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.spawnmax=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.spawndelay=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.spawnqty=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.hurtfall=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.castshadow=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.reducetexture=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.speed=t.a;
						t.a_s = c_ReadString ( 1 ); // t.entityelement[t.e].eleprof.aishoot_s=t.a_s;  //PE: Not used anymore.
						t.a_s = c_ReadString ( 1 ); t.entityelement[t.e].eleprof.hasweapon_s=t.a_s;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.lives=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].spawn.max=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].spawn.delay=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].spawn.qty=t.a;
						t.a_f = c_ReadFloat ( 1 ); t.entityelement[t.e].eleprof.scale=t.a_f;
						t.a_f = c_ReadFloat ( 1 ); t.entityelement[t.e].eleprof.coneheight=t.a_f;
						t.a_f = c_ReadFloat ( 1 ); t.entityelement[t.e].eleprof.coneangle=t.a_f;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.strength=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.isimmobile=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.cantakeweapon=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.quantity=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.markerindex=t.a;
						t.a = c_ReadLong ( 1 ); t.dw=t.a ; t.dw=t.dw+0xFF000000 ; t.entityelement[t.e].eleprof.light.color=t.dw;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.light.range=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.trigger.stylecolor=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.trigger.waypointzoneindex=t.a;
						t.a_s = c_ReadString ( 1 ); // t.entityelement[t.e].eleprof.basedecal_s=t.a_s;  //PE: Not used anymore.
					}
					if (  t.versionnumberload >= 102 ) 
					{
						//  Version 1.02
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.rateoffire=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.damage=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.accuracy=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.reloadqty=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.fireiterations=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.lifespan=t.a;
						t.a_f = c_ReadFloat ( 1 ); t.entityelement[t.e].eleprof.throwspeed=t.a_f;
						t.a_f = c_ReadFloat ( 1 ); t.entityelement[t.e].eleprof.throwangle=t.a_f;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.bounceqty=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.explodeonhit=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.weaponisammo=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.spawnupto=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.spawnafterdelay=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.spawnwhendead=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.perentityflags =t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.perentityflags =t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.perentityflags =t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.perentityflags =t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.perentityflags =t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.perentityflags =t.a;
					}
					if (  t.versionnumberload >= 103 ) 
					{
						//  Version 1.03 - V1 draft physics
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.physics=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.phyweight=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.phyfriction=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.phyforcedamage=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.rotatethrow=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.explodable=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.explodedamage=t.a;
						t.a = c_ReadLong ( 1 ); //t.entityelement[t.e].eleprof.phydw4=t.a;
						t.a = c_ReadLong ( 1 ); //t.entityelement[t.e].eleprof.phydw5=t.a;
					}
					if (  t.versionnumberload >= 104 ) 
					{
						//  Version 1.04 - BETA4 extra field
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.phyalways=t.a;
					}
					if (  t.versionnumberload >= 105 ) 
					{
						//  Version 1.05 - BETA8 extra fields
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.spawndelayrandom=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.spawnqtyrandom=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.spawnvel=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.spawnvelrandom=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.spawnangle=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.spawnanglerandom=t.a;
					}
					if (  t.versionnumberload >= 106 ) 
					{
						//  Version 1.06 - BETA10 extra fields
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.spawnatstart=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.spawnlife=t.a;
					}
					if (  t.versionnumberload >= 107 ) 
					{
						//  FPSCV104RC8 - forgot to save infinilight index (dynamic lights in final build never worked)
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.light.index=t.a;
					}
					if (  t.versionnumberload >= 199 ) 
					{
						//  X10 EXTRAS - Ignored in X9
						t.a = c_ReadLong ( 1 );
						t.a = c_ReadLong ( 1 );
						t.a = c_ReadLong ( 1 );
						t.a = c_ReadLong ( 1 );
						t.a = c_ReadLong ( 1 );
						t.a = c_ReadLong ( 1 );
						t.a = c_ReadLong ( 1 );
						t.a = c_ReadLong ( 1 );
						t.a = c_ReadLong ( 1 );
						t.a = c_ReadLong ( 1 );
						t.a = c_ReadLong ( 1 );
						t.a = c_ReadLong ( 1 );
						t.a = c_ReadLong ( 1 );
						t.a = c_ReadLong ( 1 );
						t.a = c_ReadLong ( 1 );
						t.a = c_ReadLong ( 1 );
						t.a = c_ReadLong ( 1 );
					}
					if (  t.versionnumberload >= 200 ) 
					{
						//  X10 EXTRAS 190707 - Ignored in X9
						t.a = c_ReadLong ( 1 );
						t.a = c_ReadLong ( 1 );
						t.a = c_ReadLong ( 1 );
						t.a = c_ReadLong ( 1 );
						t.a = c_ReadLong ( 1 );
						t.a = c_ReadLong ( 1 );
					}
					if (  t.versionnumberload >= 217 ) 
					{
						//  FPGC - 300710 - save new entity element data
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.particleoverride=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.particle.offsety=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.particle.scale=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.particle.randomstartx=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.particle.randomstarty=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.particle.randomstartz=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.particle.linearmotionx=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.particle.linearmotiony=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.particle.linearmotionz=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.particle.randommotionx=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.particle.randommotiony=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.particle.randommotionz=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.particle.mirrormode=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.particle.camerazshift=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.particle.scaleonlyx=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.particle.lifeincrement=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.particle.alphaintensity=t.a;
					}
					if (  t.versionnumberload >= 218 ) 
					{
						//  V118 - 060810 - knxrb - Decal animation setting (Added animation choice setting).
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.particle.animated=t.a;
					}
					if (  t.versionnumberload >= 301 ) 
					{
						//  Reloaded ALPHA 1.0045
						t.a_s = c_ReadString ( 1 ); //t.entityelement[t.e].eleprof.aiinitname_s=t.a_s; //PE: Not used anymore.
						t.a_s = c_ReadString ( 1 ); t.entityelement[t.e].eleprof.aimainname_s=t.a_s;
						t.a_s = c_ReadString ( 1 ); //t.entityelement[t.e].eleprof.aidestroyname_s=t.a_s;
						t.a_s = c_ReadString ( 1 ); //t.entityelement[t.e].eleprof.aishootname_s=t.a_s;
					}
					if (  t.versionnumberload >= 302 ) 
					{
						//  Reloaded BETA 1.005
					}
					if (  t.versionnumberload >= 303 ) 
					{
						//  Reloaded BETA 1.007
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.animspeed=t.a;
					}
					if (  t.versionnumberload >= 304 ) 
					{
						//  Reloaded BETA 1.007-200514
						t.a_f = c_ReadFloat ( 1 ); t.entityelement[t.e].eleprof.conerange=t.a_f;
					}
					if (  t.versionnumberload >= 305 ) 
					{
						//  Reloaded BETA 1.0085
						t.a_f = c_ReadFloat ( 1 ); 
						if ( t.a_f > 1e8 ) t.a_f = 0;
						t.entityelement[t.e].scalex=t.a_f;
						t.a_f = c_ReadFloat ( 1 ); 
						if ( t.a_f > 1e8 ) t.a_f = 0;
						t.entityelement[t.e].scaley=t.a_f;
						t.a_f = c_ReadFloat ( 1 ); 
						if ( t.a_f > 1e8 ) t.a_f = 0;
						t.entityelement[t.e].scalez=t.a_f;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.range=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.dropoff=t.a;
					}
					if (  t.versionnumberload >= 306 ) 
					{
						//  GameGuru 1.00.010
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.isviolent=t.a;
					}
					if (  t.versionnumberload >= 307 ) 
					{
						//  GameGuru 1.00.020
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.explodeheight =t.a;
					}
					if (  t.versionnumberload >= 308 ) 
					{
						//  GameGuru 1.01.001
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.usespotlighting=t.a;
					}
					if (  t.versionnumberload >= 309 ) 
					{
						//  GameGuru 1.01.002
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.lodmodifier=t.a;
					}
					if (  t.versionnumberload >= 310 ) 
					{
						//  GameGuru 1.133
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.isocluder=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.isocludee=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.colondeath=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.parententityindex=t.a;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.parentlimbindex=t.a;
						t.a_s = c_ReadString ( 1 ); t.entityelement[t.e].eleprof.soundset2_s=t.a_s;
						t.a_s = c_ReadString ( 1 ); t.entityelement[t.e].eleprof.soundset3_s=t.a_s;
						t.a_s = c_ReadStringIncl0xA( 1 ); t.entityelement[t.e].eleprof.soundset4_s=t.a_s;
					}
					if (  t.versionnumberload >= 311 ) 
					{
						//  GameGuru 1.133B
						t.a_f = c_ReadFloat ( 1 ); t.entityelement[t.e].eleprof.lootpercentage = t.a_f; // was specularperc
					}
					if (  t.versionnumberload >= 312 ) 
					{
						//  GameGuru 1.14 EBE
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].iHasParentIndex = t.a;
					}
					if (  t.versionnumberload >= 313 ) 
					{
						// VRQ V3
						t.a_s = c_ReadString ( 1 ); t.entityelement[t.e].eleprof.voiceset_s=t.a_s;
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.voicerate = t.a;
					}
					if (t.versionnumberload >= 314)
					{
						//PE: we need to copy t.entityprofile[t.ttentid].WEMaterial before customizing.
						int tmaster = t.entityelement[t.e].bankindex;
						if (tmaster < t.entityprofile.size())
						{
							t.entityelement[t.e].eleprof.WEMaterial = t.entityprofile[tmaster].WEMaterial;
						}

						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.bCustomWickedMaterialActive = t.a;
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.WEMaterial.MaterialActive = t.a;
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.WEMaterial.bCastShadows[0] = t.a;
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.WEMaterial.bDoubleSided[0] = t.a;
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.WEMaterial.bPlanerReflection[0] = t.a;
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.WEMaterial.bTransparency[0] = t.a;

						unsigned long ulValue = 0;
						t.a_s = c_ReadString(1);
						sscanf(t.a_s.Get(), "%lu", &ulValue);
						t.entityelement[t.e].eleprof.WEMaterial.dwBaseColor[0] = ulValue;

						t.a_s = c_ReadString(1);
						sscanf(t.a_s.Get(), "%lu", &ulValue);
						t.entityelement[t.e].eleprof.WEMaterial.dwEmmisiveColor[0] = ulValue;

						t.a_f = c_ReadFloat(1); t.entityelement[t.e].eleprof.WEMaterial.fReflectance[0] = t.a_f;

						// LB: in previous builds, only one mesh material details where stored, we can retain this for single material objects in slot 0
						t.a = c_ReadLong(1);
						int iFrameAndWEMaterialSlotIndex = 0;
						t.a_s = c_ReadString(1); t.entityelement[t.e].eleprof.WEMaterial.baseColorMapName[iFrameAndWEMaterialSlotIndex] = t.a_s;
						t.a_s = c_ReadString(1); t.entityelement[t.e].eleprof.WEMaterial.normalMapName[iFrameAndWEMaterialSlotIndex] = t.a_s;
						t.a_s = c_ReadString(1); t.entityelement[t.e].eleprof.WEMaterial.surfaceMapName[iFrameAndWEMaterialSlotIndex] = t.a_s;
						t.a_s = c_ReadString(1); t.entityelement[t.e].eleprof.WEMaterial.displacementMapName[iFrameAndWEMaterialSlotIndex] = t.a_s;
						t.a_s = c_ReadString(1); t.entityelement[t.e].eleprof.WEMaterial.emissiveMapName[iFrameAndWEMaterialSlotIndex] = t.a_s;
						t.a_s = c_ReadString(1);
						#ifndef DISABLEOCCLUSIONMAP
						t.entityelement[t.e].eleprof.WEMaterial.occlusionMapName[iFrameAndWEMaterialSlotIndex] = t.a_s;
						#endif
						t.a_f = c_ReadFloat(1); t.entityelement[t.e].eleprof.WEMaterial.fNormal[iFrameAndWEMaterialSlotIndex] = t.a_f;
						t.a_f = c_ReadFloat(1); t.entityelement[t.e].eleprof.WEMaterial.fRoughness[iFrameAndWEMaterialSlotIndex] = t.a_f;
						t.a_f = c_ReadFloat(1); t.entityelement[t.e].eleprof.WEMaterial.fMetallness[iFrameAndWEMaterialSlotIndex] = t.a_f;
						t.a_f = c_ReadFloat(1); t.entityelement[t.e].eleprof.WEMaterial.fEmissive[iFrameAndWEMaterialSlotIndex] = t.a_f;
						t.a_f = c_ReadFloat(1); t.entityelement[t.e].eleprof.WEMaterial.fAlphaRef[iFrameAndWEMaterialSlotIndex] = t.a_f;
					}
					if (t.versionnumberload >= 315)
					{
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.light.fLightHasProbe = t.a;
					}
					if (t.versionnumberload >= 316)
					{
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.iObjectLinkID = t.a;
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.iCharAlliance = t.a;
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.iCharFaction = t.a;
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.iObjectReserved1 = t.a;
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.iObjectReserved2 = t.a;
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.iObjectReserved3 = t.a;
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.iCharPatrolMode = t.a;
						t.a = c_ReadFloat(1); t.entityelement[t.e].eleprof.fCharRange[0] = t.a;
						t.a = c_ReadFloat(1); t.entityelement[t.e].eleprof.fCharRange[1] = t.a;
						for (int i = 0;i < 10;i++)
						{
							t.a = c_ReadFloat(1); t.entityelement[t.e].eleprof.fObjectDataReserved[i] = t.a;
							t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.iObjectRelationships[i] = t.a;
							t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.iObjectRelationshipsType[i] = t.a;
							t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.iObjectRelationshipsData[i] = t.a;
						}
					}
					if (t.versionnumberload >= 317)
					{
						for (int i = 1; i < MAXMESHMATERIALS; i++)
						{
							t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.WEMaterial.bCastShadows[i] = t.a;
							t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.WEMaterial.bDoubleSided[i] = t.a;
							t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.WEMaterial.bPlanerReflection[i] = t.a;
							t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.WEMaterial.bTransparency[i] = t.a;

							unsigned long ulValue = 0;
							t.a_s = c_ReadString(1);
							sscanf(t.a_s.Get(), "%lu", &ulValue);
							t.entityelement[t.e].eleprof.WEMaterial.dwBaseColor[i] = ulValue;

							t.a_s = c_ReadString(1);
							sscanf(t.a_s.Get(), "%lu", &ulValue);
							t.entityelement[t.e].eleprof.WEMaterial.dwEmmisiveColor[i] = ulValue;

							t.a_f = c_ReadFloat(1); t.entityelement[t.e].eleprof.WEMaterial.fReflectance[i] = t.a_f;

							int iFrameAndWEMaterialSlotIndex = i;
							t.a_s = c_ReadString(1); t.entityelement[t.e].eleprof.WEMaterial.baseColorMapName[iFrameAndWEMaterialSlotIndex] = t.a_s;
							t.a_s = c_ReadString(1); t.entityelement[t.e].eleprof.WEMaterial.normalMapName[iFrameAndWEMaterialSlotIndex] = t.a_s;
							t.a_s = c_ReadString(1); t.entityelement[t.e].eleprof.WEMaterial.surfaceMapName[iFrameAndWEMaterialSlotIndex] = t.a_s;
							t.a_s = c_ReadString(1); t.entityelement[t.e].eleprof.WEMaterial.displacementMapName[iFrameAndWEMaterialSlotIndex] = t.a_s;
							t.a_s = c_ReadString(1); t.entityelement[t.e].eleprof.WEMaterial.emissiveMapName[iFrameAndWEMaterialSlotIndex] = t.a_s;
							t.a_s = c_ReadString(1);
							#ifndef DISABLEOCCLUSIONMAP
							t.entityelement[t.e].eleprof.WEMaterial.occlusionMapName[iFrameAndWEMaterialSlotIndex] = t.a_s;
							#endif
							t.a_f = c_ReadFloat(1); t.entityelement[t.e].eleprof.WEMaterial.fNormal[iFrameAndWEMaterialSlotIndex] = t.a_f;
							t.a_f = c_ReadFloat(1); t.entityelement[t.e].eleprof.WEMaterial.fRoughness[iFrameAndWEMaterialSlotIndex] = t.a_f;
							t.a_f = c_ReadFloat(1); t.entityelement[t.e].eleprof.WEMaterial.fMetallness[iFrameAndWEMaterialSlotIndex] = t.a_f;
							t.a_f = c_ReadFloat(1); t.entityelement[t.e].eleprof.WEMaterial.fEmissive[iFrameAndWEMaterialSlotIndex] = t.a_f;
							t.a_f = c_ReadFloat(1); t.entityelement[t.e].eleprof.WEMaterial.fAlphaRef[iFrameAndWEMaterialSlotIndex] = t.a_f;
						}
					}
					if (t.versionnumberload >= 318)
					{
						t.a = c_ReadFloat(1); t.entityelement[t.e].eleprof.WEMaterial.fRenderOrderBias[0] = t.a;
						for (int i = 1; i < MAXMESHMATERIALS; i++)
						{
							t.a = c_ReadFloat(1); t.entityelement[t.e].eleprof.WEMaterial.fRenderOrderBias[i] = t.a;
						}
					}
					if (t.versionnumberload >= 319)
					{
						//PE: The same group data is stored under ALL t.e and get reset each time.
						//PE: So level with 1000 objects , WILL setup groups and load ALL group images 1000 times ?
						//PE: We cant change old level now, so use this hack.
						//PE: Also this was leaking mem , not sure where. anyway this hack fix it. New maps will only save it under t.e == 1
						extern int g_iUniqueGroupID;
						t.a = c_ReadLong(1); 
						if (t.a > 0) g_iUniqueGroupID = t.a; // strange bug that resets this t.a to zero, the writer dumped in zeros!!
						int iNumberOfGroups = 0;
						t.a = c_ReadLong(1); iNumberOfGroups = t.a;
						for (int gi = 0; gi < iNumberOfGroups; gi++)
						{
							if (bFirstTimeOnlyToGrabGroupDataOops == true) vEntityGroupList[gi].clear();
							int iItemsInThisGroup = 0;
							t.a = c_ReadLong(1); iItemsInThisGroup = t.a;
							for (int i = 0; i < iItemsInThisGroup; i++)
							{
								sRubberBandType item;
								t.a = c_ReadLong(1); item.iGroupID = t.a;
								t.a = c_ReadLong(1); item.iParentGroupID = t.a;
								t.a = c_ReadLong(1); item.e = t.a;
								t.a = c_ReadFloat(1); item.x = t.a;
								t.a = c_ReadFloat(1); item.y = t.a;
								t.a = c_ReadFloat(1); item.z = t.a;
								t.a = c_ReadFloat(1); item.quatAngle.x = t.a;
								t.a = c_ReadFloat(1); item.quatAngle.y = t.a;
								t.a = c_ReadFloat(1); item.quatAngle.z = t.a;
								t.a = c_ReadFloat(1); item.quatAngle.w = t.a;
								if (bFirstTimeOnlyToGrabGroupDataOops == true) vEntityGroupList[gi].push_back(item);
							}
						}
						// and load in group thumb images, and load them into the iEntityGroupListImage image list (so can see them in groups tab)
						extern int iEntityGroupListImage[MAXGROUPSLISTS];
						for (int gi = 0; gi < iNumberOfGroups; gi++)
						{
							if (bFirstTimeOnlyToGrabGroupDataOops==true) iEntityGroupListImage[gi] = 0;
							t.a = c_ReadLong(1); int iHasImage = t.a;
							if (iHasImage == 1 && bFirstTimeOnlyToGrabGroupDataOops == true)
							{
								char pGroupImgFilename[MAX_PATH];
								sprintf(pGroupImgFilename, "%sgroupimg%d.png", g.mysystem.levelBankTestMap_s.Get(), gi);
								if (FileExist(pGroupImgFilename) == 1)
								{
									//Find free image id.
									int iImageID = 0;
									for (int i = 0; i < MAXGROUPSLISTS; i++)
									{
										bool bAlreadyUsed = false;
										int iNewImageID = (110000 + 9000) + i;// (g.perentitypromptimageoffset + 9000) + i;
										for (int l = MAXGROUPSLISTS; l > 0; l--)
										{
											if (iEntityGroupListImage[l] == iNewImageID)
											{
												bAlreadyUsed = true;
												break;
											}
										}
										if (!bAlreadyUsed)
										{
											iImageID = iNewImageID;
											break;
										}
									}
									if (iImageID != 0)
									{
										image_setlegacyimageloading(true);
										LoadImage(pGroupImgFilename, iImageID);
										image_setlegacyimageloading(false);
										iEntityGroupListImage[gi] = iImageID;
									}
								}
							}
						}

						// subsequent entities will just read the data but do nothing with vEntityGroupList!
						bFirstTimeOnlyToGrabGroupDataOops = false;
					}

					if (t.versionnumberload >= 320)
					{
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.newparticle.bParticle_Preview = t.a;
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.newparticle.bParticle_Show_At_Start = t.a;
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.newparticle.bParticle_Looping_Animation = t.a;
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.newparticle.bParticle_Full_Screen = t.a;
						t.a_f = c_ReadFloat(1); t.entityelement[t.e].eleprof.newparticle.fParticle_Fullscreen_Duration = t.a_f;
						t.a_f = c_ReadFloat(1); t.entityelement[t.e].eleprof.newparticle.fParticle_Fullscreen_Fadein = t.a_f;
						t.a_f = c_ReadFloat(1); t.entityelement[t.e].eleprof.newparticle.fParticle_Fullscreen_Fadeout = t.a_f;
						t.a_s = c_ReadString(1); t.entityelement[t.e].eleprof.newparticle.Particle_Fullscreen_Transition = t.a_s;
						t.a_f = c_ReadFloat(1); t.entityelement[t.e].eleprof.newparticle.fParticle_Speed = t.a_f;
						t.a_f = c_ReadFloat(1); t.entityelement[t.e].eleprof.newparticle.fParticle_Opacity = t.a_f;
					}
					if (t.versionnumberload >= 321)
					{
						t.a_s = c_ReadString(1); t.entityelement[t.e].eleprof.newparticle.emittername= t.a_s;
					}
					if (t.versionnumberload >= 322)
					{
						t.a_f = c_ReadFloat(1); t.entityelement[t.e].fDecalSpeed = t.a_f;
						t.a_f = c_ReadFloat(1); t.entityelement[t.e].fDecalOpacity = t.a_f;
					}
					if (t.versionnumberload >= 323)
					{
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.iOverrideCollisionMode = t.a;
					}
					if (t.versionnumberload >= 324)
					{
						t.a_f = c_ReadFloat(1); t.entityelement[t.e].eleprof.weapondamagemultiplier = t.a_f;
						t.a_f = c_ReadFloat(1); t.entityelement[t.e].eleprof.meleedamagemultiplier = t.a_f;
					}
					if (t.versionnumberload >= 325)
					{
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.iAffectedByGravity = t.a;
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.iMoveSpeed = t.a;
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.iTurnSpeed = t.a;
					}

					if (t.versionnumberload >= 326)
					{
						t.a = c_ReadLong ( 1 ); t.entityelement[t.e].eleprof.light.offsetup =t.a; //Store spot radius.
					}
					if (t.versionnumberload >= 327)
					{
						t.a_s = c_ReadString(1); t.entityelement[t.e].eleprof.soundset5_s = t.a_s;
						t.a_s = c_ReadString(1); t.entityelement[t.e].eleprof.soundset6_s = t.a_s;
					}
					if (t.versionnumberload >= 328)
					{
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.iUseSoundVariants = t.a;
					}
					if (t.versionnumberload >= 329)
					{
						t.a = c_ReadLong(1); t.entityelement[t.e].quatmode = t.a;
						t.a_f = c_ReadFloat(1); t.entityelement[t.e].quatx = t.a_f;
						t.a_f = c_ReadFloat(1); t.entityelement[t.e].quaty = t.a_f;
						t.a_f = c_ReadFloat(1); t.entityelement[t.e].quatz = t.a_f;
						t.a_f = c_ReadFloat(1); t.entityelement[t.e].quatw = t.a_f;
					}
					if (t.versionnumberload >= 330)
					{
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.bAutoFlatten = t.a;
					}
					if (t.versionnumberload >= 331)
					{
						t.a_s = c_ReadString(1); t.entityelement[t.e].eleprof.overrideanimset_s = t.a_s;
					}
					if (t.versionnumberload >= 332)
					{
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.iscollectable = t.a;
					}
					if (t.versionnumberload >= 333)
					{
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.iSwimSpeed = t.a;
					}
					if (t.versionnumberload >= 334)
					{
						extern cstr sEntityGroupListName[MAXGROUPSLISTS];
						int iNumberOfGroups = c_ReadLong(1);
						for (int gi = 0; gi < iNumberOfGroups; gi++)
						{
							t.a_s = c_ReadString(1);
							sEntityGroupListName[gi] = t.a_s;
						}
					}
					if (t.versionnumberload >= 335)
					{
						t.a = c_ReadLong(1); t.entityelement[t.e].creationOfGroupID = t.a;
					}
					if (t.versionnumberload >= 336)
					{
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.light.fLightHasProbeX = t.a;
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.light.fLightHasProbeY = t.a;
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.light.fLightHasProbeZ = t.a;
					}
					if (t.versionnumberload >= 337)
					{
						t.a = c_ReadLong(1); t.entityelement[t.e].iCanGoUnderwater = t.a;
					}
					if (t.versionnumberload >= 338)
					{
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.clipcapacity = t.a;
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.weaponpropres1 = t.a;
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.weaponpropres2 = t.a;
					}
					if (t.versionnumberload >= 339)
					{
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.WEMaterial.customShaderID = t.a;
						t.a = t.a_f = c_ReadFloat(1); t.entityelement[t.e].eleprof.WEMaterial.customShaderParam1 = t.a_f;
						t.a = t.a_f = c_ReadFloat(1); t.entityelement[t.e].eleprof.WEMaterial.customShaderParam2 = t.a_f;
						t.a = t.a_f = c_ReadFloat(1); t.entityelement[t.e].eleprof.WEMaterial.customShaderParam3 = t.a_f;
						t.a = t.a_f = c_ReadFloat(1); t.entityelement[t.e].eleprof.WEMaterial.customShaderParam4 = t.a_f;
						t.a = t.a_f = c_ReadFloat(1); t.entityelement[t.e].eleprof.WEMaterial.customShaderParam5 = t.a_f;
						t.a = t.a_f = c_ReadFloat(1); t.entityelement[t.e].eleprof.WEMaterial.customShaderParam6 = t.a_f;
						t.a = t.a_f = c_ReadFloat(1); t.entityelement[t.e].eleprof.WEMaterial.customShaderParam7 = t.a_f;
						t.a_s = c_ReadString(1); t.entityelement[t.e].eleprof.explodable_decalname = t.a_s;
					}
					if (t.versionnumberload >= 340)
					{
						//PE: For next version add: used_old_particle_effect. bindtoMeshID.
						t.a_s = c_ReadString(1); t.entityelement[t.e].eleprof.WEMaterial.WPEffect = t.a_s;
						//PE: Add some fillers we can use later.
						float fFiller;
						int iFiller;
						cstr sFiller;
						
						t.a = t.a_f = c_ReadFloat(1); t.entityelement[t.e].eleprof.light.fProbeBrightness = t.a_f;

						t.a = c_ReadLong(1); t.entityelement[t.e].iAllowBuletHole = t.a;
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.iMaterialSoundIndex = t.a;
						
						t.a = t.a_f = c_ReadFloat(1); fFiller = t.a_f;
						t.a = t.a_f = c_ReadFloat(1); fFiller = t.a_f;
						t.a = c_ReadLong(1);
						t.entityelement[t.e].eleprof.systemwide_lua = t.a;
						if (t.entityelement[t.e].eleprof.systemwide_lua > 1)
							t.entityelement[t.e].eleprof.systemwide_lua = 0;
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.isobjective_alwaysactive = t.a;
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.isProjectGlobal = t.a;
						t.a_s = c_ReadString(1); sFiller = t.a_s;
						t.a_s = c_ReadString(1); sFiller = t.a_s;
						t.a_s = c_ReadString(1); sFiller = t.a_s;
					}
					if (t.versionnumberload >= 341)
					{
						t.a = c_ReadLong(1); t.entityelement[t.e].eleprof.bUseFPESettings = t.a;
					}
					if (t.versionnumberload >= 342)
					{
						t.a_s = c_ReadString(1); t.entityelement[t.e].eleprof.soundset4a_s = t.a_s;
					}

					// get the index of the entity profile
					t.ttentid=t.entityelement[t.e].bankindex;
					if (t.ttentid >= t.entityprofile.size())
					{
						// somehow, the entity profile bank index was corrupted
						t.ttentid = 0;
						t.entityelement[t.e].bankindex = 0;
						continue;
					}

					// if added element already exists with same parent in list, we do not need to add it
					if (g_iAddEntityElementsMode == 1 && t.entityelement[t.e].bankindex > 0 )
					{
						bool bElementExists = false;
						for (int finde = 1; finde <= g.entityelementlist; finde++)
						{
							if (finde != t.e && t.entityelement[finde].bankindex == t.entityelement[t.e].bankindex)
							{
								bElementExists = true;
								break;
							}
						}
						if (bElementExists == true)
						{
							// we do not keep this element!
							t.ttentid = 0;
							t.entityelement[t.e].bankindex = 0;
							t.entityelement[t.e].specialentityloadflag = 0;
							// and step back so this list size does not run away from us
							if (bIncreasedListSize == true)
							{
								g.entityelementlist--;
							}
							// go to next element in ELE sequence
							continue;
						}
					}

					if (t.entityelement[t.e].eleprof.light.fProbeBrightness == 0)
						t.entityelement[t.e].eleprof.light.fProbeBrightness = 1.0f;

					// fill in the blanks if load older version
					if (  t.versionnumberload<103 ) 
					{
						//  Version 1.03 - V1 draft physics (-1 means calculate at entobj-loadtime)
						t.entityelement[t.e].eleprof.physics=t.entityprofile[t.ttentid].physics;
						t.entityelement[t.e].eleprof.phyweight=t.entityprofile[t.ttentid].phyweight;
						t.entityelement[t.e].eleprof.phyfriction=t.entityprofile[t.ttentid].phyfriction;
						t.entityelement[t.e].eleprof.phyforcedamage=t.entityprofile[t.ttentid].phyforcedamage;
						t.entityelement[t.e].eleprof.rotatethrow=t.entityprofile[t.ttentid].rotatethrow;
						t.entityelement[t.e].eleprof.explodable=t.entityprofile[t.ttentid].explodable;
						//t.entityelement[t.e].eleprof.phydw3=0;
						//t.entityelement[t.e].eleprof.phydw4=0;
						//t.entityelement[t.e].eleprof.phydw5=0;
					}
					if (  t.versionnumberload<104 ) 
					{
						//  Version 1.04 - BETA4 extra field
						t.entityelement[t.e].eleprof.phyalways=t.entityprofile[t.ttentid].phyalways;
					}
					if (  t.versionnumberload<105 ) 
					{
						//  Version 1.05 - BETA8
						t.entityelement[t.e].eleprof.spawndelayrandom=t.entityprofile[t.ttentid].spawndelayrandom;
						t.entityelement[t.e].eleprof.spawnqtyrandom=t.entityprofile[t.ttentid].spawnqtyrandom;
						t.entityelement[t.e].eleprof.spawnvel=t.entityprofile[t.ttentid].spawnvel;
						t.entityelement[t.e].eleprof.spawnvelrandom=t.entityprofile[t.ttentid].spawnvelrandom;
						t.entityelement[t.e].eleprof.spawnangle=t.entityprofile[t.ttentid].spawnangle;
						t.entityelement[t.e].eleprof.spawnanglerandom=t.entityprofile[t.ttentid].spawnanglerandom;
					}
					if (  t.versionnumberload<106 ) 
					{
						//  Version 1.06 - BETA10
						t.entityelement[t.e].eleprof.spawnatstart=t.entityprofile[t.ttentid].spawnatstart;
						t.entityelement[t.e].eleprof.spawnlife=t.entityprofile[t.ttentid].spawnlife;
					}
					if (  t.versionnumberload<217 ) 
					{
						//  FPGC - 300710 - older levels dont use particle override
						t.entityelement[t.e].eleprof.particleoverride=0;
					}
					if (  t.versionnumberload<303 ) 
					{
						//  Reloaded BETA 1.007
						t.entityelement[t.e].eleprof.animspeed=t.entityprofile[t.ttentid].animspeed;
					}
					if (  t.versionnumberload<304 ) 
					{
						//  Reloaded BETA 1.007-200514
						t.entityelement[t.e].eleprof.conerange=t.entityprofile[t.ttentid].conerange;
					}
					if (  t.versionnumberload<306 ) 
					{
						//  GameGuru 1.00.010
						t.entityelement[t.e].eleprof.isviolent=t.entityprofile[t.ttentid].isviolent;
					}
					if (  t.versionnumberload<307 ) 
					{
						//  GameGuru 1.00.020
						t.entityelement[t.e].eleprof.explodeheight =t.entityprofile[t.ttentid].explodeheight;
					}
					if (  t.versionnumberload<310 ) 
					{
						//  GameGuru 1.133
						t.entityelement[t.e].eleprof.isocluder=t.entityprofile[t.ttentid].isocluder;
						t.entityelement[t.e].eleprof.isocludee=t.entityprofile[t.ttentid].isocludee;
						t.entityelement[t.e].eleprof.colondeath=t.entityprofile[t.ttentid].colondeath;
						t.entityelement[t.e].eleprof.parententityindex=t.entityprofile[t.ttentid].parententityindex;
						t.entityelement[t.e].eleprof.parentlimbindex=t.entityprofile[t.ttentid].parentlimbindex;
						t.entityelement[t.e].eleprof.soundset2_s=t.entityprofile[t.ttentid].soundset2_s;
						t.entityelement[t.e].eleprof.soundset3_s=t.entityprofile[t.ttentid].soundset3_s;
						t.entityelement[t.e].eleprof.soundset4_s=t.entityprofile[t.ttentid].soundset4_s;
					}
					if (  t.versionnumberload<311 ) 
					{
						//  GameGuru 1.133B
						t.entityelement[t.e].eleprof.lootpercentage =t.entityprofile[t.ttentid].lootpercentage;
					}
					if (  t.versionnumberload<312 ) 
					{
						//  GameGuru 1.14 EBE
						t.entityelement[t.e].iHasParentIndex = 0;
					}
					if (  t.versionnumberload < 313 ) 
					{
						// VRQ V3
						t.entityelement[t.e].eleprof.voiceset_s=t.entityprofile[t.ttentid].voiceset_s;
						t.entityelement[t.e].eleprof.voicerate=t.entityprofile[t.ttentid].voicerate;
					}
					if (t.versionnumberload < 330)
					{
						t.entityelement[t.e].eleprof.bAutoFlatten = false;
					}
					if (t.versionnumberload < 341)
					{
						//PE: Default to false on old levels , so we don't overwrite users custom settings.
						//PE: if !bCustomWickedMaterialActive we can use bUseFPESettings.
						if (t.entityelement[t.e].eleprof.bCustomWickedMaterialActive)
							t.entityelement[t.e].eleprof.bUseFPESettings = false;
						else
							t.entityelement[t.e].eleprof.bUseFPESettings = true;

						t.entityelement[t.e].eleprof.iMaterialSoundIndex = 0;
					}
					//t.entityelement[t.e].entitydammult_f=1.0; not used any more, reused field for iCanGoUnderwater and renamed entitydammult_f to reserved2
					//t.entityelement[t.e].entityacc=1.0;

					// 131115 - transparency control was removed from GG properties IDE, so ensure
					// it reflects the latest entity profile information (until we allow this value back in)
					t.entityelement[t.e].eleprof.transparency = t.entityprofile[t.ttentid].transparency;
				}
			}
		}
		else
		{
			t.failedtoload=1;
		}
		c_CloseFile (  1 );

		// can change field values here if updates to engine move vital resources
		// MAY2025 - moved all default animation files to animations\set folder so can centrally
		// add new animations and all existing and new characters/logic can take advantage of new ones
		for (t.e = 1; t.e <= g.entityelementlist; t.e++)
		{
			if (t.entityelement[t.e].bankindex>0)
			{
				if ( t.entityelement[t.e].eleprof.overrideanimset_s.Len()>0)
				{
					char pFileLocation[MAX_PATH];
					strcpy(pFileLocation, t.entityelement[t.e].eleprof.overrideanimset_s.Get());
					LPSTR pPartLocation = strstr(pFileLocation, "charactercreatorplus\\parts\\");
					if (pPartLocation != NULL)
					{
						pPartLocation+=strlen("charactercreatorplus\\");
						*pPartLocation = '\0';
						char pDefAnimFileLocation[MAX_PATH];
						strcpy(pDefAnimFileLocation, pFileLocation);
						strcat(pDefAnimFileLocation, "animations\\sets\\");
						strcat(pDefAnimFileLocation, pPartLocation+strlen("parts") + 1);
						t.entityelement[t.e].eleprof.overrideanimset_s = pDefAnimFileLocation;
					}
				}
			}
		}

		// If replacement file active, can swap in new SCRIPT and SOUND references
		if(g_iAddEntityElementsMode==0)
		{
			if (Len(t.editor.replacefilepresent_s.Get()) > 1)
			{
				// now go through ELEPROF enrties to update any SCRIPTBANK references and SOUNDSET references
				for (t.e = 1; t.e <= g.entityelementlist; t.e++)
				{
					for (t.tcheck = 1; t.tcheck <= 9; t.tcheck++)
					{
						if (t.tcheck == 1)  t.tcheck_s = t.entityelement[t.e].eleprof.aimain_s;
						if (t.tcheck == 2)  t.tcheck_s = t.entityelement[t.e].eleprof.soundset_s;
						if (t.tcheck == 3)  t.tcheck_s = t.entityelement[t.e].eleprof.soundset1_s;
						if (t.tcheck == 4)  t.tcheck_s = t.entityelement[t.e].eleprof.soundset2_s;
						if (t.tcheck == 5)  t.tcheck_s = t.entityelement[t.e].eleprof.soundset3_s;
						if (t.tcheck == 6)  t.tcheck_s = t.entityelement[t.e].eleprof.soundset4_s;
						if (t.tcheck == 7)  t.tcheck_s = t.entityelement[t.e].eleprof.soundset5_s;
						if (t.tcheck == 8)  t.tcheck_s = t.entityelement[t.e].eleprof.soundset6_s;
						if (t.tcheck == 9)  t.tcheck_s = t.entityelement[t.e].eleprof.soundset4a_s;
						t.ttry_s = "";
						for (t.nn = 1; t.nn <= Len(t.tcheck_s.Get()); t.nn++)
						{
							t.ttry_s = t.ttry_s + Mid(t.tcheck_s.Get(), t.nn);
							if ((cstr(Mid(t.tcheck_s.Get(), t.nn)) == "\\" && cstr(Mid(t.tcheck_s.Get(), t.nn + 1)) == "\\") || (cstr(Mid(t.tcheck_s.Get(), t.nn)) == "/" && cstr(Mid(t.tcheck_s.Get(), t.nn + 1)) == "/"))
							{
								++t.nn;
							}
						}
						t.ttry_s = Lower(t.ttry_s.Get());
						for (t.tt = 1; t.tt <= t.treplacementmax; t.tt++)
						{
							if (t.replacements_s[t.tt][0] == t.ttry_s)
							{
								//  found entry we can replace
								if (t.tcheck == 1) { t.entityelement[t.e].eleprof.aimain_s = t.replacements_s[t.tt][1]; t.tt = t.treplacementmax + 1; }
								if (t.tcheck == 2) { t.entityelement[t.e].eleprof.soundset_s = t.replacements_s[t.tt][1]; t.tt = t.treplacementmax + 1; }
								if (t.tcheck == 3) { t.entityelement[t.e].eleprof.soundset1_s = t.replacements_s[t.tt][1]; t.tt = t.treplacementmax + 1; }
								if (t.tcheck == 4) { t.entityelement[t.e].eleprof.soundset2_s = t.replacements_s[t.tt][1]; t.tt = t.treplacementmax + 1; }
								if (t.tcheck == 5) { t.entityelement[t.e].eleprof.soundset3_s = t.replacements_s[t.tt][1]; t.tt = t.treplacementmax + 1; }
								if (t.tcheck == 6) { t.entityelement[t.e].eleprof.soundset4_s = t.replacements_s[t.tt][1]; t.tt = t.treplacementmax + 1; }
								if (t.tcheck == 7) { t.entityelement[t.e].eleprof.soundset5_s = t.replacements_s[t.tt][1]; t.tt = t.treplacementmax + 1; }
								if (t.tcheck == 8) { t.entityelement[t.e].eleprof.soundset6_s = t.replacements_s[t.tt][1]; t.tt = t.treplacementmax + 1; }
								if (t.tcheck == 9) { t.entityelement[t.e].eleprof.soundset4a_s = t.replacements_s[t.tt][1]; t.tt = t.treplacementmax + 1; }
							}
						}
					}
				}
				//  free usages
				UnDim (t.replacements_s);
			}
		}
	}

	// and erase any elements that DO NOT have a valid profile (file moved/deleted)
	if ( t.failedtoload == 1 ) 
	{
		//  FPGC - 270410 - if entity binary from X10 (or just not supported), ensure NO entities!
		g.entityelementlist=0;
		g.entityelementmax=0;
	}
	else
	{
		// clean up for grouplist corruption
		for (int gi = 0; gi < MAXGROUPSLISTS; gi++)
		{
			if (vEntityGroupList[gi].size() == 0)
			{
				sEntityGroupListName[gi] = "";
			}
		}

		// clean up for entityelement corruption
		if (g_iAddEntityElementsMode == 0)
		{
			for (t.e = 1; t.e <= g.entityelementlist; t.e++)
			{
				t.entid = t.entityelement[t.e].bankindex;
				if (t.entid > 0)
				{
					if (t.entid > ArrayCount(t.entitybank_s))
					{
						t.entityelement[t.e].bankindex = 0;
					}
					else
					{
						if (Len(t.entitybank_s[t.entid].Get()) == 0)
						{
							//  030715 - but only erase if entity not a marker
							if (t.entityprofile[t.entid].ismarker == 0)
							{
								t.entityelement[t.e].bankindex = 0;
							}
						}
					}
				}
			}
		}

		// could have group data corruption, storing entityelements that are not in the level
		int iNumberOfGroups = MAXGROUPSLISTS;
		for (int gi = 0; gi < iNumberOfGroups; gi++)
		{
			int iItemsInThisGroup = vEntityGroupList[gi].size();
			for (int i = 0; i < iItemsInThisGroup; i++)
			{
				int e = vEntityGroupList[gi][i].e;
				if (e > 0 && e <= g.entityelementlist)
				{
					if (t.entityelement[e].bankindex == 0)
					{
						vEntityGroupList[gi].erase(vEntityGroupList[gi].begin() + i);
						iItemsInThisGroup--;
						i--;
					}
				}
			}
		}

		// could have entities in more than one group, remove them from duplicates
		for (int gi = 0; gi < iNumberOfGroups; gi++)
		{
			int iItemsInThisGroup = vEntityGroupList[gi].size();
			for (int i = 0; i < iItemsInThisGroup; i++)
			{
				int e = vEntityGroupList[gi][i].e;
				if (e > 0 && e <= g.entityelementlist)
				{
					for (int gi2 = 0; gi2 < iNumberOfGroups; gi2++)
					{
						if (gi2 != gi)
						{
							for (int i2 = 0; i2 < vEntityGroupList[gi2].size(); i2++)
							{
								if (vEntityGroupList[gi2][i2].e == e)
								{
									vEntityGroupList[gi2].erase(vEntityGroupList[gi2].begin() + i2);
									i2--;
								}
							}
						}
					}
				}
			}
		}
	}
}
#endif

void entity_loadelementsdata(void)
{
	// fast loading of ELE file
	char pLoadEntityData[MAX_PATH];
	sprintf(pLoadEntityData, "c_entity_loadelementsdata: %s", t.elementsfilename_s.Get());
	timestampactivity(0, pLoadEntityData);
	char pLoadEntityDataBefore[MAX_PATH];
	sprintf(pLoadEntityDataBefore, "c_entity_loadelementsdata before: %d", g.entityelementmax);
	timestampactivity(0, pLoadEntityDataBefore);
	c_entity_loadelementsdata();

	extern bool bKeepWindowsResponding;
	void EmptyMessages(void);
	if (bKeepWindowsResponding)
		EmptyMessages();

	char pLoadEntityDataAfter[MAX_PATH];
	sprintf(pLoadEntityDataAfter, "c_entity_loadelementsdata after: %d", g.entityelementmax);
	timestampactivity(0, pLoadEntityDataAfter);
	
	//PE: Add any systemwidelua.ele to end of current elements
	extern StoryboardStruct Storyboard;
	if (strlen(Storyboard.gamename) > 0)
	{
		timestampactivity(0, "loading in systemwidelua.ele");
		cstr storeoldELEfile = t.elementsfilename_s;
		char collectionELEfilename[MAX_PATH];
		strcpy(collectionELEfilename, "projectbank\\");
		strcat(collectionELEfilename, Storyboard.gamename);
		strcat(collectionELEfilename, "\\systemwidelua.ele");
		t.elementsfilename_s = collectionELEfilename;
		extern int g_iAddEntityElementsMode;
		g_iAddEntityElementsMode = 2;
		c_entity_loadelementsdata();

		if (bKeepWindowsResponding)
			EmptyMessages();

		t.elementsfilename_s = storeoldELEfile;
		g_iAddEntityElementsMode = 0;

		//PE: Need to load masterobject if not there.
		for (int i = 1; i <= g.entityelementlist; i++)
		{
			if (t.entityelement[i].eleprof.systemwide_lua)
			{
				int tentid = t.entityelement[i].bankindex;
				if (tentid > 0)
				{
					if (tentid == 0 || tentid > t.entityprofile.size() || t.entityprofile[tentid].ismarker != 12)
					{
						//PE: Need to reload and remap.
						extern int g_iAddEntitiesModeFrom;
						g_iAddEntitiesModeFrom = g.entidmaster + 1;
						cstr entProfileToAdd_s = "_markers\\BehaviorHidden.fpe";

						int iFoundMatchEntID = 0;
						for (int entid = 1; entid <= g.entidmaster; entid++)
						{
							if (stricmp(t.entitybank_s[entid].Get(), entProfileToAdd_s.Get()) == NULL)
							{
								iFoundMatchEntID = entid;
								break;
							}
						}
						if (iFoundMatchEntID == 0)
						{
							g.entidmaster++;
							entity_validatearraysize();
							t.entitybank_s[g.entidmaster] = entProfileToAdd_s;
							iFoundMatchEntID = g.entidmaster;
							extern int g_iAddEntitiesMode;
							g_iAddEntitiesMode = 1;
							entity_loadentitiesnow();

							if (bKeepWindowsResponding)
								EmptyMessages();

							g_iAddEntitiesMode = 0;
						}
						t.entityelement[i].bankindex = iFoundMatchEntID;
					}
				}
			}
		}
	}
	sprintf(pLoadEntityDataAfter, "c_entity_loadelementsdata after systemwidelua.ele: %d", g.entityelementmax);
	timestampactivity(0, pLoadEntityDataAfter);

	// now entitybank and entityelement and vEntityGroupList are loaded, we need to refresh any smart objects (they may have changed externally, i.e BE)
	bool bForceSmartObjectRefresh = true;
	if (bForceSmartObjectRefresh == true)
	{
		for (int iGroupIndex = 0; iGroupIndex < MAXGROUPSLISTS; iGroupIndex++)
		{
			if (vEntityGroupList[iGroupIndex].size() > 0)
			{
				int iUniqueGroupID = vEntityGroupList[iGroupIndex][0].iGroupID;
				if (iUniqueGroupID > 0)
				{
					// load a fresh copy of this smart object (will ultimately call LoadGroup to populate with needed elements for below)
					int iFoundEntID = 0;
					for (int entIndex = 1; entIndex <= g.entidmaster; entIndex++)
					{
						cstr tmp = cstr("entitybank\\") + t.entitybank_s[entIndex];
						int iSmartObjectGroupIndex = GetGroupIndexFromName(tmp);
						if (iSmartObjectGroupIndex >= 0 && iSmartObjectGroupIndex < MAXGROUPSLISTS)
						{
							if (vEntityGroupList[iSmartObjectGroupIndex].size() > 0)
							{
								if (vEntityGroupList[iSmartObjectGroupIndex][0].iGroupID == iUniqueGroupID)
								{
									iFoundEntID = entIndex;
									break;
								}
							}
						}
					}

					if (bKeepWindowsResponding)
						EmptyMessages();

					extern void ReloadEntityIDInSitu(int);
					ReloadEntityIDInSitu (iFoundEntID);
				}
			}
		}
	}

	// after all entity profiles and elements in, can refresh collection list that references entities
	if(g_collectionLabels.size()>0)
	{
		// before
		char pLogCollectionCountBefore[MAX_PATH];
		sprintf(pLogCollectionCountBefore, "Collection list size before: %d", g_collectionList.size());
		timestampactivity(0, pLogCollectionCountBefore);
		bool bLoadingLevel = true;

		if (bKeepWindowsResponding)
			EmptyMessages();

		if (refresh_collection_from_entities(bLoadingLevel) == true)
		{
			// after
			char pLogCollectionCount[MAX_PATH];
			sprintf(pLogCollectionCount, "Collection list size after: %d", g_collectionList.size());
			timestampactivity(0, pLogCollectionCount);

			// refresh detected some entity profile/elements are missing
			// these will be needed for multi-level consistency and carrying items around the whole game
			timestampactivity(0, "Loading additional entities for collection item list");

			// build list of required entity profiles
			timestampactivity(0, "entity bank additions");
			std::vector<int> g_entityBankAdditionsCollectionIndex;
			std::vector<cstr> g_entityBankAdditions;
			for (int n = 0; n < g_collectionList.size(); n++)
			{
				if (g_collectionList[n].collectionFields.size() > 1)
				{
					LPSTR pCollectionItemTitle = g_collectionList[n].collectionFields[0].Get();
					LPSTR pCollectionItemProfile = g_collectionList[n].collectionFields[1].Get();
					if (strlen(pCollectionItemTitle) > 0)
					{
						if (g_collectionList[n].iEntityID == 0)
						{
							bool bFoundIt = false;
							if (stricmp(g_collectionList[n].collectionFields[1].Get(), "default") == NULL)
							{
								// try desc as a clue to finding it
								for (int entid = 1; entid <= g.entidmastermax; entid++)
								{
									if (stricmp (t.entityprofileheader[entid].desc_s.Get(), pCollectionItemTitle) == NULL)
									{
										g_collectionList[n].collectionFields[1] = t.entitybank_s[entid];
										g_collectionList[n].iEntityID = entid;
										bFoundIt = true;
										break;
									}
								}
								pCollectionItemProfile = "";
							}
							if(bFoundIt==false && stricmp(g_collectionList[n].collectionFields[1].Get(), "none") == NULL)
							{
								// None could be a weapon
								if (g_collectionList[n].collectionFields.size() > 8)
								{
									if (strnicmp(g_collectionList[n].collectionFields[8].Get(), "weapon=", 7) == NULL)
									{
										LPSTR pWeaponName = g_collectionList[n].collectionFields[8].Get() + 7;
										for (int entid = 1; entid <= g.entidmastermax; entid++)
										{
											if (stricmp (t.entityprofile[entid].isweapon_s.Get(), pWeaponName) == NULL)
											{
												g_collectionList[n].collectionFields[1] = t.entitybank_s[entid];
												g_collectionList[n].iEntityID = entid;
												bFoundIt = true;
												break;
											}
										}
										if (bFoundIt == false)
										{
											// okay, so no weapon of this type in the parent objects, need to point to the
											// entity using the gun collection database
											for (int gunid = 1; gunid <= g.gunmax; gunid++)
											{
												if (stricmp (t.gun[gunid].name_s.Get(), pWeaponName) == NULL)
												{
													if (t.gun[gunid].pathtostockentity_s.Len() > 0)
													{
														g_collectionList[n].collectionFields[1] = t.gun[gunid].pathtostockentity_s;
													}
												}
											}
										}
									}
								}
							}
							if(bFoundIt==false)
							{
								// do a direct search for it
								LPSTR pCollectionItemProfile = g_collectionList[n].collectionFields[1].Get();
								if (strlen(pCollectionItemProfile) > 0)
								{
									for (int entid = 1; entid <= g.entidmastermax; entid++)
									{
										if (stricmp (t.entitybank_s[entid].Get(), pCollectionItemProfile) == NULL)
										{
											g_collectionList[n].iEntityID = entid;
											bFoundIt = true;
											break;
										}
									}
								}
							}
							if (bFoundIt == false && strlen(pCollectionItemProfile) > 0)
							{
								//PE: "none" will end up as a ebe missing file.
								if( strlen(pCollectionItemProfile) > 1 && !(stricmp(pCollectionItemProfile, "none") == NULL) )
								{
									// add entity to additions
									g_entityBankAdditions.push_back(pCollectionItemProfile);
									g_entityBankAdditionsCollectionIndex.push_back(n);

									// and log it
									char pLogCollectionAdded[MAX_PATH];
									sprintf(pLogCollectionAdded, "entity Bank Addition: %s", pCollectionItemProfile);
									timestampactivity(0, pLogCollectionAdded);
								}
							}
						}
					}
				}
			}

			// merge load entity profiles
			if (g_entityBankAdditions.size() > 0)
			{
				extern int g_iAddEntitiesModeFrom;
				g_iAddEntitiesModeFrom = g.entidmaster + 1;
				for (int i = 0; i < g_entityBankAdditions.size(); i++)
				{
					// Look for this
					cstr entProfileToAdd_s = g_entityBankAdditions[i];

					// add if not exist in bank
					int iFoundMatchEntID = 0;
					for (int entid = 1; entid <= g.entidmaster - 1; entid++)
					{
						if (stricmp(t.entitybank_s[entid].Get(), entProfileToAdd_s.Get()) == NULL)
						{
							iFoundMatchEntID = entid;
							break;
						}
					}
					if (iFoundMatchEntID == 0)
					{
						g.entidmaster++;
						entity_validatearraysize ();
						t.entitybank_s[g.entidmaster] = entProfileToAdd_s;
						iFoundMatchEntID = g.entidmaster;
					}
					g_collectionList[g_entityBankAdditionsCollectionIndex[i]].iEntityID = iFoundMatchEntID;
				}
				extern int g_iAddEntitiesMode;
				g_iAddEntitiesMode = 1;
				entity_loadentitiesnow();
				if (bKeepWindowsResponding)
					EmptyMessages();
				g_iAddEntitiesMode = 0;
			}

			// load in game project elements file to end of current elements
			timestampactivity(0, "loading in collection - items.ele");
			cstr storeoldELEfile = t.elementsfilename_s;
			char collectionELEfilename[MAX_PATH];
			strcpy(collectionELEfilename, "projectbank\\");
			extern StoryboardStruct Storyboard;
			strcat(collectionELEfilename, Storyboard.gamename);
			strcat(collectionELEfilename, "\\collection - items.ele");
			t.elementsfilename_s = collectionELEfilename;
			extern int g_iAddEntityElementsMode;
			g_iAddEntityElementsMode = 1;
			c_entity_loadelementsdata();

			if (bKeepWindowsResponding)
				EmptyMessages();

			t.elementsfilename_s = storeoldELEfile;
			g_iAddEntityElementsMode = 0;

			// associate new entity elements with collection entry
			for (int e = 1; e <= g.entityelementlist; e++)
			{
				if (t.entityelement[e].specialentityloadflag == 123)
				{
					// if already exists, do NOT add, just wipe out
					t.entityelement[e].specialentityloadflag = 0;
					bool bAlreadyExistsSoSkip = false;
					for (int eee = 1; eee <= g.entityelementmax; eee++)
					{
						if (eee != e)
						{
							if (t.entityelement[eee].bankindex > 0)
							{
								LPSTR pEntityElementName1 = t.entityelement[e].eleprof.name_s.Get();
								LPSTR pEntityElementName2 = t.entityelement[eee].eleprof.name_s.Get();
								if (stricmp(pEntityElementName1, pEntityElementName2) == NULL)
								{
									t.entityelement[e].maintype = 0;
									t.entityelement[e].bankindex = 0;
									bAlreadyExistsSoSkip = true;
									break;
								}
							}
						}
					}
					int iCollectionIndexFound = -1;
					if (bAlreadyExistsSoSkip == false)
					{
						LPSTR pEntityElementName = t.entityelement[e].eleprof.name_s.Get();
						for (int n = 0; n < g_collectionList.size(); n++)
						{
							if (g_collectionList[n].collectionFields.size() > 1)
							{
								LPSTR pCollectionItemTitle = g_collectionList[n].collectionFields[0].Get();
								if (stricmp(pCollectionItemTitle, pEntityElementName) == NULL)
								{
									// the newly loaded element will be used for this collection item
									g_collectionList[n].iEntityElementE = e;

									// now hide it away in the level
									t.entityelement[e].bankindex = g_collectionList[n].iEntityID;
									t.entityelement[e].x = -99999;
									t.entityelement[e].y = -99999;
									t.entityelement[e].z = -99999;
									t.entityelement[e].eleprof.spawnatstart = 1;

									// found, so can quit
									iCollectionIndexFound = n;
									break;
								}
							}
						}
					}

					// now scan all entities in common with this collection item entity just loaded from the ELE file
					// and clone all details to them (there should only be one collectale entity element/eleprof identity)
					int iDeletingThisElementSoUseFoundE = 0;
					bool bDeleteThisNewElementNotNeeded = false;
					LPSTR pMasterEntityName = t.entityelement[e].eleprof.name_s.Get();
					for (int ee = 1; ee <= g.entityelementmax; ee++)
					{
						if (ee != e)
						{
							int masterid = t.entityelement[ee].bankindex;
							if (masterid > 0)
							{
								if (stricmp (t.entityelement[ee].eleprof.name_s.Get(), pMasterEntityName) == NULL)
								{
									t.entityelement[ee].eleprof = t.entityelement[e].eleprof;
									iDeletingThisElementSoUseFoundE = ee;
									bDeleteThisNewElementNotNeeded = true;
								}
							}
						}
					}
					if (bDeleteThisNewElementNotNeeded == true)
					{
						// delete element, not needed for this level
						t.entityelement[e].bankindex = 0;

						// and reassign collection item E to the existing one
						if (iCollectionIndexFound != -1)
						{
							g_collectionList[iCollectionIndexFound].iEntityElementE = iDeletingThisElementSoUseFoundE;
						}
					}
				}
			}

			// and now if any 'iEntityElementE' are zero, we need to create an element off-level 
			// as an instance that can be cloned down the road
			bool bSaveUpdatedELEFile = false;
			for (int n = 0; n < g_collectionList.size(); n++)
			{
				if (g_collectionList[n].iEntityID > 0 && g_collectionList[n].iEntityElementE == 0)
				{
					// before make an instance, check if it already exists as a valid entity
					int entid = g_collectionList[n].iEntityID;
					for (int ee = 1; ee <= g.entityelementmax; ee++)
					{
						int masterid = t.entityelement[ee].bankindex;
						if (masterid > 0)
						{
							if (masterid == entid)
							{
								g_collectionList[n].iEntityElementE = ee;
								break;
							}
						}
					}
					if (g_collectionList[n].iEntityElementE == 0)
					{
						// make a hidden instance of this entity
						t.gridentity = entid;
						t.gridentityeditorfixed = 0;
						t.entitymaintype = 1;
						t.entitybankindex = t.entid;
						t.gridentitystaticmode = 0;
						t.gridentityhasparent = 0;
						t.gridentityposx_f = 0;
						t.gridentityposz_f = 0;
						t.gridentityposy_f = 0;
						t.gridentityrotatex_f = 0;
						t.gridentityrotatey_f = 0;
						t.gridentityrotatez_f = 0;
						t.gridentityrotatequatmode = 1;
						t.gridentityrotatequatx_f = 0;
						t.gridentityrotatequaty_f = 0;
						t.gridentityrotatequatz_f = 0;
						t.gridentityrotatequatw_f = 1;
						t.gridentityscalex_f = 100;
						t.gridentityscaley_f = 100;
						t.gridentityscalez_f = 100;
						t.entid = entid; entity_fillgrideleproffromprofile();
						entity_addentitytomap ();

						if (bKeepWindowsResponding)
							EmptyMessages();

						t.e = t.tupdatee;
						t.entityelement[t.e].x = -99999;
						t.entityelement[t.e].y = -99999;
						t.entityelement[t.e].z = -99999;
						t.entityelement[t.e].eleprof.spawnatstart = 0;
						t.entityelement[t.e].lua.firsttime = 0;
						t.entityelement[t.e].active = 0;
						g_collectionList[n].iEntityElementE = t.e;
						t.gridentity = 0;
					}
					bSaveUpdatedELEFile = true;
				}
			}
			// also if iEntityID is zero at this point, find it and update
			for (int n = 0; n < g_collectionList.size(); n++)
			{
				if (g_collectionList[n].iEntityID == 0)
				{
					if (g_collectionList[n].collectionFields.size() > 1)
					{
						LPSTR pCollectionItemTitle = g_collectionList[n].collectionFields[0].Get();
						for (int eee = 1; eee <= g.entityelementmax; eee++)
						{
							if (t.entityelement[eee].bankindex > 0)
							{
								LPSTR pEntityElementName = t.entityelement[eee].eleprof.name_s.Get();
								if (stricmp(pCollectionItemTitle, pEntityElementName) == NULL)
								{
									// the newly loaded element will be used for this collection item
									g_collectionList[n].iEntityElementE = eee;
									bSaveUpdatedELEFile = true;
									break;
								}
							}
						}
					}
				}
			}
			if (bSaveUpdatedELEFile == true)
			{
				// ensure collection list and ELE file up to date
				extern preferences pref;
				save_rpg_system(pref.cLastUsedStoryboardProject, true);
				if (bKeepWindowsResponding)
					EmptyMessages();

			}

			// also to fix older saves, remove any off-level duplicates
			for (int e = 1; e <= g.entityelementlist; e++)
			{
				if (t.entityelement[e].bankindex > 0)
				{
					bool bFoundDuplicate = false;
					for (int eee = 1; eee <= g.entityelementmax; eee++)
					{
						if (eee != e)
						{
							if (t.entityelement[e].bankindex > 0)
							{
								if (t.entityelement[e].bankindex == t.entityelement[eee].bankindex)
								{
									if (t.entityelement[e].x == -99999 && t.entityelement[e].y == -99999 && t.entityelement[e].z == -99999)
									{
										if (t.entityelement[eee].x == -99999 && t.entityelement[eee].y == -99999 && t.entityelement[eee].z == -99999)
										{
											bFoundDuplicate = true;
											int iPreferredElementE = 0;
											for (int n = 0; n < g_collectionList.size(); n++)
											{
												if (g_collectionList[n].iEntityID == t.entityelement[e].bankindex)
												{
													if (g_collectionList[n].iEntityElementE > 0)
													{
														iPreferredElementE = g_collectionList[n].iEntityElementE;
														break;
													}
												}
											}
											if (iPreferredElementE > 0)
											{
												if (eee != iPreferredElementE)
												{
													// if not preferred element, remove duplicate from element list
													t.entityelement[eee].maintype = 0;
													t.entityelement[eee].bankindex = 0;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}

			// and a full pass to convert any parent objects into collectables if the collection list has them
			refresh_rpg_parents_of_items();
			if (bKeepWindowsResponding)
				EmptyMessages();

		}
	}

	// so it seems vEntityGroupList can be saved in elements data, but reference entities that no longer exist
	// suggesting too a deeper issue relating to group data becoming corrupt, but cannot allow group datra to proceed
	// that points to entities that do not exist, so must delete those rogue groups
	bool bCleanUpRedundantGroupData = true;
	if (bCleanUpRedundantGroupData == true)
	{
		for (int iGroupIndex = 0; iGroupIndex < MAXGROUPSLISTS; iGroupIndex++)
		{
			if (vEntityGroupList[iGroupIndex].size() > 0)
			{
				for (int i = 0; i < vEntityGroupList[iGroupIndex].size(); i++)
				{
					int e = vEntityGroupList[iGroupIndex][i].e;
					if (e == 0 || e >= t.entityelement.size())
					{
						// this entity is zero or does not exist in entity element data
						// so remove entire group as redundant/corrupt
						vEntityGroupList[iGroupIndex].clear();
						break;
					}
				}
			}
		}
	}
}

// class to write in two passes, first adds up total size, second creates and fills the data buffer
class EntityWriter
{
protected:

	unsigned char* pData = 0;
	unsigned int iDataSize = 0;
	unsigned int iMaxDataSize = 0;
	unsigned int doWrite = 0; // first pass = 0, second pass = 1

public:

	EntityWriter() {}
	~EntityWriter() { if ( pData ) delete [] pData; }

	unsigned char* GetData() { return pData; }
	unsigned int GetDataSize() { return iDataSize; }

	// call between passes
	void AllocateDataForWrite()
	{
		assert( !pData ); 
		if ( pData ) delete [] pData; // should't be called more than once, but prevent memory leak if it is
		pData = new unsigned char[ iDataSize ];
		iMaxDataSize = iDataSize;
		iDataSize = 0;
		doWrite = 1;
	}

	void WriteLong( int num ) 
	{
		const unsigned int elementSize = sizeof(int);
		if ( !doWrite ) iDataSize += elementSize;
		else
		{
			assert( (iDataSize + elementSize) <= iMaxDataSize );
			memcpy( pData + iDataSize, &num, elementSize );
			iDataSize += elementSize;
		}
	}
	
	void WriteFloat( float num ) 
	{
		const unsigned int elementSize = sizeof(float);
		if ( !doWrite ) iDataSize += elementSize;
		else
		{
			assert( (iDataSize + elementSize) <= iMaxDataSize );
			memcpy( pData + iDataSize, &num, elementSize );
			iDataSize += elementSize;
		}
	}

	void WriteString( char* str ) 
	{
		for (int i = 0; i < strlen(str); i++)
		{
			//PE: Make sure we dont break the .ele file , seen some corrupt strings with \n\r.
			if (*(str + i) == '\n' || *(str + i) == '\r')
			{
				*(str + i) = ' ';
			}
		}
		unsigned int elementSize = strlen(str);
		if ( !doWrite ) iDataSize += elementSize + 2;
		else
		{
			if ( elementSize )
			{
				assert( (iDataSize + elementSize) <= iMaxDataSize );
				memcpy( pData + iDataSize, str, elementSize );
				iDataSize += elementSize;
			}

			pData[ iDataSize ] = 13;
			pData[ iDataSize + 1 ] = 10;
			iDataSize += 2;
		}
	}
	void WriteStringInclude0xa(char* str)
	{
		for (int i = 0; i < strlen(str); i++)
		{
			//PE: Make sure we dont break the .ele file , seen some corrupt strings with \n\r.
			if ( *(str + i) == '\r')
			{
				*(str + i) = ' ';
			}
		}
		unsigned int elementSize = strlen(str);
		if (!doWrite) iDataSize += elementSize + 2;
		else
		{
			if (elementSize)
			{
				assert((iDataSize + elementSize) <= iMaxDataSize);
				memcpy(pData + iDataSize, str, elementSize);
				iDataSize += elementSize;
			}

			pData[iDataSize] = 13;
			pData[iDataSize + 1] = 10;
			iDataSize += 2;
		}
	}

};

void entity_saveelementsdata (bool bForCollectionELE)
{
	//  Uses elementfilename$
	if ( t.elementsfilename_s == ""  )  t.elementsfilename_s = g.mysystem.levelBankTestMap_s+"map.ele";

	//  Reduce list size if later elements blank
	int temp = g.entityelementlist;
	while ( temp > 0 ) 
	{
		if ( t.entityelement[temp].maintype == 0  )  --temp; else break;
	}
	g.entityelementlist = temp;

	//  Save entity element list
	t.versionnumbersave = 342;

	EntityWriter writer;

	// write in two passes, first adds up total data size, second pass allocates and writes to the buffer
	for( int pass = 0; pass < 2; pass++ )
	{
		writer.WriteLong ( t.versionnumbersave );
		writer.WriteLong ( g.entityelementlist );
		if ( g.entityelementlist>0 ) 
		{
			for ( int ent = 1 ; ent<=  g.entityelementlist; ent++ )
			{
				if ( t.versionnumbersave >= 101 ) 
				{
					//  Version 1.01 - EA
					writer.WriteLong( t.entityelement[ent].maintype );
					writer.WriteLong( t.entityelement[ent].bankindex );
					writer.WriteLong( t.entityelement[ent].staticflag );
					writer.WriteFloat( t.entityelement[ent].x );
					writer.WriteFloat( t.entityelement[ent].y );
					writer.WriteFloat( t.entityelement[ent].z );
					writer.WriteFloat( t.entityelement[ent].rx );
					writer.WriteFloat( t.entityelement[ent].ry );
					writer.WriteFloat( t.entityelement[ent].rz );
					writer.WriteString( t.entityelement[ent].eleprof.name_s.Get() );
					writer.WriteString ( "" );
					writer.WriteString( t.entityelement[ent].eleprof.aimain_s.Get() );
					writer.WriteString ( "" );
					writer.WriteLong( t.entityelement[ent].eleprof.isobjective );
					writer.WriteString( t.entityelement[ent].eleprof.usekey_s.Get() );
					writer.WriteString( t.entityelement[ent].eleprof.ifused_s.Get() );
					writer.WriteString ( "" );
					writer.WriteLong( t.entityelement[ent].eleprof.uniqueelement );
					writer.WriteString( t.entityelement[ent].eleprof.texd_s.Get() );
					writer.WriteString( t.entityelement[ent].eleprof.texaltd_s.Get() );
					writer.WriteString( t.entityelement[ent].eleprof.effect_s.Get() );
					writer.WriteLong( t.entityelement[ent].eleprof.transparency );
					writer.WriteLong( t.entityelement[ent].editorfixed );
					writer.WriteString( t.entityelement[ent].eleprof.soundset_s.Get() );
					writer.WriteString( t.entityelement[ent].eleprof.soundset1_s.Get() );
					writer.WriteLong( t.entityelement[ent].eleprof.spawnmax );
					writer.WriteLong( t.entityelement[ent].eleprof.spawndelay );
					writer.WriteLong( t.entityelement[ent].eleprof.spawnqty );
					writer.WriteLong( t.entityelement[ent].eleprof.hurtfall );
					writer.WriteLong( t.entityelement[ent].eleprof.castshadow );
					writer.WriteLong( t.entityelement[ent].eleprof.reducetexture );
					writer.WriteLong( t.entityelement[ent].eleprof.speed );
					writer.WriteString ( "" );
					writer.WriteString( t.entityelement[ent].eleprof.hasweapon_s.Get() );
					writer.WriteLong( t.entityelement[ent].eleprof.lives );
					writer.WriteLong( t.entityelement[ent].spawn.max );
					writer.WriteLong( t.entityelement[ent].spawn.delay );
					writer.WriteLong( t.entityelement[ent].spawn.qty );
					writer.WriteFloat( t.entityelement[ent].eleprof.scale );
					writer.WriteFloat( t.entityelement[ent].eleprof.coneheight );
					writer.WriteFloat( t.entityelement[ent].eleprof.coneangle );
					writer.WriteLong( t.entityelement[ent].eleprof.strength );
					writer.WriteLong( t.entityelement[ent].eleprof.isimmobile );
					writer.WriteLong( t.entityelement[ent].eleprof.cantakeweapon );
					writer.WriteLong( t.entityelement[ent].eleprof.quantity );
					writer.WriteLong( t.entityelement[ent].eleprof.markerindex );
					writer.WriteLong( t.entityelement[ent].eleprof.light.color & 0x00FFFFFF );
					writer.WriteLong( t.entityelement[ent].eleprof.light.range );
					writer.WriteLong( t.entityelement[ent].eleprof.trigger.stylecolor );
					writer.WriteLong( t.entityelement[ent].eleprof.trigger.waypointzoneindex );
					writer.WriteString ( "" );
				}
				if ( t.versionnumbersave >= 102 ) 
				{
					writer.WriteLong( t.entityelement[ent].eleprof.rateoffire );
					writer.WriteLong( t.entityelement[ent].eleprof.damage );
					writer.WriteLong( t.entityelement[ent].eleprof.accuracy );
					writer.WriteLong( t.entityelement[ent].eleprof.reloadqty );
					writer.WriteLong( t.entityelement[ent].eleprof.fireiterations );
					writer.WriteLong( t.entityelement[ent].eleprof.lifespan );
					writer.WriteFloat( t.entityelement[ent].eleprof.throwspeed );
					writer.WriteFloat( t.entityelement[ent].eleprof.throwangle );
					writer.WriteLong( t.entityelement[ent].eleprof.bounceqty );
					writer.WriteLong( t.entityelement[ent].eleprof.explodeonhit );
					writer.WriteLong( t.entityelement[ent].eleprof.weaponisammo );
					writer.WriteLong( t.entityelement[ent].eleprof.spawnupto );
					writer.WriteLong( t.entityelement[ent].eleprof.spawnafterdelay );
					writer.WriteLong( t.entityelement[ent].eleprof.spawnwhendead );
					writer.WriteLong( t.entityelement[ent].eleprof.perentityflags );
					writer.WriteLong( t.entityelement[ent].eleprof.perentityflags );
					writer.WriteLong( t.entityelement[ent].eleprof.perentityflags );
					writer.WriteLong( t.entityelement[ent].eleprof.perentityflags );
					writer.WriteLong( t.entityelement[ent].eleprof.perentityflags );
					writer.WriteLong( t.entityelement[ent].eleprof.perentityflags );
				}
				if ( t.versionnumbersave >= 103 ) 
				{
					//  V1 first draft - physics
					writer.WriteLong( t.entityelement[ent].eleprof.physics );
					writer.WriteLong( t.entityelement[ent].eleprof.phyweight );
					writer.WriteLong( t.entityelement[ent].eleprof.phyfriction );
					writer.WriteLong( t.entityelement[ent].eleprof.phyforcedamage );
					writer.WriteLong( t.entityelement[ent].eleprof.rotatethrow );
					writer.WriteLong( t.entityelement[ent].eleprof.explodable );
					writer.WriteLong( t.entityelement[ent].eleprof.explodedamage );
					writer.WriteLong( 0 );
					writer.WriteLong( 0 );
				}
				if ( t.versionnumbersave >= 104 ) 
				{
					//  Addition of new physics field for BETA4
					writer.WriteLong( t.entityelement[ent].eleprof.phyalways );
				}
				if ( t.versionnumbersave >= 105 ) 
				{
					//  Addition of new spawn fields for BETA8
					writer.WriteLong( t.entityelement[ent].eleprof.spawndelayrandom );
					writer.WriteLong( t.entityelement[ent].eleprof.spawnqtyrandom );
					writer.WriteLong( t.entityelement[ent].eleprof.spawnvel );
					writer.WriteLong( t.entityelement[ent].eleprof.spawnvelrandom );
					writer.WriteLong( t.entityelement[ent].eleprof.spawnangle );
					writer.WriteLong( t.entityelement[ent].eleprof.spawnanglerandom );
				}
				if ( t.versionnumbersave >= 106 ) 
				{
					//  Addition of new fields for BETA10
					writer.WriteLong( t.entityelement[ent].eleprof.spawnatstart );
					writer.WriteLong( t.entityelement[ent].eleprof.spawnlife );
				}
				if ( t.versionnumbersave >= 107 ) 
				{
					//  FPSCV104RC8 - forgot to save infinilight index (dynamic lights in final build never worked)
					writer.WriteLong( t.entityelement[ent].eleprof.light.index );
				}
				if ( t.versionnumbersave >= 199 ) 
				{
					//  X10 specific version - any new save data must be higher than 200
					writer.WriteLong ( 0 );
					writer.WriteLong ( 0 );
					writer.WriteLong ( 0 );
					writer.WriteLong ( 0 );
					writer.WriteLong ( 0 );
					writer.WriteLong ( 0 );
					writer.WriteLong ( 0 );
					writer.WriteLong ( 0 );
					writer.WriteLong ( 0 );
					writer.WriteLong ( 0 );
					writer.WriteLong ( 0 );
					writer.WriteLong ( 0 );
					writer.WriteLong ( 0 );
					writer.WriteLong ( 0 );
					writer.WriteLong ( 0 );
					writer.WriteLong ( 0 );
					writer.WriteLong ( 0 );
				}
				if ( t.versionnumbersave >= 200 ) 
				{
					//  X10 specific version - any new save data must be higher than 200
					writer.WriteLong ( 0 );
					writer.WriteLong ( 0 );
					writer.WriteLong ( 0 );
					writer.WriteLong ( 0 );
					writer.WriteLong ( 0 );
					writer.WriteLong ( 0 );
				}
				if ( t.versionnumbersave >= 217 ) 
				{
					//  FPGC - 300710 - save new entity element data
					writer.WriteLong( t.entityelement[ent].eleprof.particleoverride );
					writer.WriteLong( t.entityelement[ent].eleprof.particle.offsety );
					writer.WriteLong( t.entityelement[ent].eleprof.particle.scale );
					writer.WriteLong( t.entityelement[ent].eleprof.particle.randomstartx );
					writer.WriteLong( t.entityelement[ent].eleprof.particle.randomstarty );
					writer.WriteLong( t.entityelement[ent].eleprof.particle.randomstartz );
					writer.WriteLong( t.entityelement[ent].eleprof.particle.linearmotionx );
					writer.WriteLong( t.entityelement[ent].eleprof.particle.linearmotiony );
					writer.WriteLong( t.entityelement[ent].eleprof.particle.linearmotionz );
					writer.WriteLong( t.entityelement[ent].eleprof.particle.randommotionx );
					writer.WriteLong( t.entityelement[ent].eleprof.particle.randommotiony );
					writer.WriteLong( t.entityelement[ent].eleprof.particle.randommotionz );
					writer.WriteLong( t.entityelement[ent].eleprof.particle.mirrormode );
					writer.WriteLong( t.entityelement[ent].eleprof.particle.camerazshift );
					writer.WriteLong( t.entityelement[ent].eleprof.particle.scaleonlyx );
					writer.WriteLong( t.entityelement[ent].eleprof.particle.lifeincrement );
					writer.WriteLong( t.entityelement[ent].eleprof.particle.alphaintensity );
				}
				if ( t.versionnumbersave >= 218 ) 
				{
					//  V118 - 060810 - knxrb - Decal animation setting (Added animation choice setting).
					writer.WriteLong( t.entityelement[ent].eleprof.particle.animated );
				}
				if ( t.versionnumbersave >= 301 ) 
				{
					//  Reloaded ALPHA 1.0045
					writer.WriteString ( "" );
					writer.WriteString( t.entityelement[ent].eleprof.aimainname_s.Get() );
					writer.WriteString ( "" );
					writer.WriteString ( "" );
				}
				if ( t.versionnumbersave >= 302 ) 
				{
					//  Reloaded BETA 1.005
				}
				if ( t.versionnumbersave >= 303 ) 
				{
					//  Reloaded BETA 1.007
					writer.WriteLong( t.entityelement[ent].eleprof.animspeed );
				}
				if ( t.versionnumbersave >= 304 ) 
				{
					//  Reloaded BETA 1.007-200514
					writer.WriteFloat( t.entityelement[ent].eleprof.conerange );
				}
				if ( t.versionnumbersave >= 305 ) 
				{
					//  Reloaded BETA 1.009
					writer.WriteFloat( t.entityelement[ent].scalex );
					writer.WriteFloat( t.entityelement[ent].scaley );
					writer.WriteFloat( t.entityelement[ent].scalez );
					writer.WriteLong( t.entityelement[ent].eleprof.range );
					writer.WriteLong( t.entityelement[ent].eleprof.dropoff );
				}
				if ( t.versionnumbersave >= 306 ) 
				{
					//  Guru 1.00.010
					writer.WriteLong( t.entityelement[ent].eleprof.isviolent );
				}
				if ( t.versionnumbersave >= 307 ) 
				{
					//  Guru 1.00.020
					writer.WriteLong( t.entityelement[ent].eleprof.explodeheight);
				}
				if ( t.versionnumbersave >= 308 ) 
				{
					//  Guru 1.01.001
					writer.WriteLong( t.entityelement[ent].eleprof.usespotlighting );
				}
				if ( t.versionnumbersave >= 309 )
				{
					//  Guru 1.01.002
					writer.WriteLong( t.entityelement[ent].eleprof.lodmodifier );
				}
				if ( t.versionnumbersave >= 310 )
				{
					//  Guru 1.133
					writer.WriteLong( t.entityelement[ent].eleprof.isocluder );
					writer.WriteLong( t.entityelement[ent].eleprof.isocludee );
					writer.WriteLong( t.entityelement[ent].eleprof.colondeath );
					writer.WriteLong( t.entityelement[ent].eleprof.parententityindex );
					writer.WriteLong( t.entityelement[ent].eleprof.parentlimbindex );
					writer.WriteString( t.entityelement[ent].eleprof.soundset2_s.Get() );
					writer.WriteString( t.entityelement[ent].eleprof.soundset3_s.Get() );
					writer.WriteStringInclude0xa( t.entityelement[ent].eleprof.soundset4_s.Get() );
				}
				if ( t.versionnumbersave >= 311 )
				{
					//  Guru 1.133B
					writer.WriteFloat( t.entityelement[ent].eleprof.lootpercentage);
				}
				if ( t.versionnumbersave >= 312 )
				{
					//  Guru 1.14 EBE
					writer.WriteLong( t.entityelement[ent].iHasParentIndex );
				}			
				if ( t.versionnumbersave >= 313 )
				{
					// VRQ V3
					writer.WriteString( t.entityelement[ent].eleprof.voiceset_s.Get() );
					writer.WriteLong( t.entityelement[ent].eleprof.voicerate );
				}
				if (t.versionnumbersave >= 314)
				{
					//PE: We must save custom "Materials" here.

					writer.WriteLong( t.entityelement[ent].eleprof.bCustomWickedMaterialActive );
					writer.WriteLong( t.entityelement[ent].eleprof.WEMaterial.MaterialActive );
					writer.WriteLong( t.entityelement[ent].eleprof.WEMaterial.bCastShadows[0] );
					writer.WriteLong( t.entityelement[ent].eleprof.WEMaterial.bDoubleSided[0] );
					writer.WriteLong( t.entityelement[ent].eleprof.WEMaterial.bPlanerReflection[0] );
					writer.WriteLong( t.entityelement[ent].eleprof.WEMaterial.bTransparency[0] );

					char tas[256];
					sprintf(tas, "%lu", (unsigned long) t.entityelement[ent].eleprof.WEMaterial.dwBaseColor[0] );
					writer.WriteString( tas );
					sprintf(tas, "%lu", (unsigned long)t.entityelement[ent].eleprof.WEMaterial.dwEmmisiveColor[0] );
					writer.WriteString( tas );

					writer.WriteFloat( t.entityelement[ent].eleprof.WEMaterial.fReflectance[0] );

					// header 9*4 = 36 bytes for each entry.
					// per mesh 11 * 4 = 44 bytes for a empty material.
					// LB: in previous builds, only one mesh material details where stored, we can retain this for single material objects in slot 0
					writer.WriteLong( 0 );
					int iFrameAndWEMaterialSlotIndex = 0;
					writer.WriteString( t.entityelement[ent].eleprof.WEMaterial.baseColorMapName[iFrameAndWEMaterialSlotIndex].Get() );
					writer.WriteString( t.entityelement[ent].eleprof.WEMaterial.normalMapName[iFrameAndWEMaterialSlotIndex].Get() );
					writer.WriteString( t.entityelement[ent].eleprof.WEMaterial.surfaceMapName[iFrameAndWEMaterialSlotIndex].Get() );
					writer.WriteString( t.entityelement[ent].eleprof.WEMaterial.displacementMapName[iFrameAndWEMaterialSlotIndex].Get() );
					writer.WriteString( t.entityelement[ent].eleprof.WEMaterial.emissiveMapName[iFrameAndWEMaterialSlotIndex].Get() );
					#ifndef DISABLEOCCLUSIONMAP
					writer.WriteString ( t.entityelement[ent].eleprof.WEMaterial.occlusionMapName[iFrameAndWEMaterialSlotIndex].Get() );
					#else
					writer.WriteString ( "" );
					#endif
					writer.WriteFloat( t.entityelement[ent].eleprof.WEMaterial.fNormal[iFrameAndWEMaterialSlotIndex] );
					writer.WriteFloat( t.entityelement[ent].eleprof.WEMaterial.fRoughness[iFrameAndWEMaterialSlotIndex] );
					writer.WriteFloat( t.entityelement[ent].eleprof.WEMaterial.fMetallness[iFrameAndWEMaterialSlotIndex] );
					writer.WriteFloat( t.entityelement[ent].eleprof.WEMaterial.fEmissive[iFrameAndWEMaterialSlotIndex] );
					writer.WriteFloat( t.entityelement[ent].eleprof.WEMaterial.fAlphaRef[iFrameAndWEMaterialSlotIndex] );
				}
				if (t.versionnumbersave >= 315)
				{
					writer.WriteLong( t.entityelement[ent].eleprof.light.fLightHasProbe );
				}
				if (t.versionnumbersave >= 316)
				{
					writer.WriteLong( t.entityelement[ent].eleprof.iObjectLinkID );
					writer.WriteLong( t.entityelement[ent].eleprof.iCharAlliance );
					writer.WriteLong( t.entityelement[ent].eleprof.iCharFaction );
					writer.WriteLong( t.entityelement[ent].eleprof.iObjectReserved1 );
					writer.WriteLong( t.entityelement[ent].eleprof.iObjectReserved2 );
					writer.WriteLong( t.entityelement[ent].eleprof.iObjectReserved3 );
					writer.WriteLong( t.entityelement[ent].eleprof.iCharPatrolMode );
					writer.WriteFloat( t.entityelement[ent].eleprof.fCharRange[0] );
					writer.WriteFloat( t.entityelement[ent].eleprof.fCharRange[1] );
					for (int i = 0; i < 10; i++)
					{
						writer.WriteFloat( t.entityelement[ent].eleprof.fObjectDataReserved[i] );
						writer.WriteLong( t.entityelement[ent].eleprof.iObjectRelationships[i] );
						writer.WriteLong( t.entityelement[ent].eleprof.iObjectRelationshipsType[i] );
						writer.WriteLong( t.entityelement[ent].eleprof.iObjectRelationshipsData[i] );
					}
				}
				if (t.versionnumbersave >= 317)
				{
					//PE: Save per mesh material settings.
					for (int i = 1; i < MAXMESHMATERIALS; i++)
					{
						writer.WriteLong( t.entityelement[ent].eleprof.WEMaterial.bCastShadows[i] );
						writer.WriteLong( t.entityelement[ent].eleprof.WEMaterial.bDoubleSided[i] );
						writer.WriteLong( t.entityelement[ent].eleprof.WEMaterial.bPlanerReflection[i] );
						writer.WriteLong( t.entityelement[ent].eleprof.WEMaterial.bTransparency[i] );

						char tas[256];
						sprintf(tas, "%lu", (unsigned long)t.entityelement[ent].eleprof.WEMaterial.dwBaseColor[i] );
						writer.WriteString( tas );
						sprintf(tas, "%lu", (unsigned long)t.entityelement[ent].eleprof.WEMaterial.dwEmmisiveColor[i] );
						writer.WriteString( tas );

						writer.WriteFloat( t.entityelement[ent].eleprof.WEMaterial.fReflectance[i] );

						int iFrameAndWEMaterialSlotIndex = i;
						writer.WriteString( t.entityelement[ent].eleprof.WEMaterial.baseColorMapName[iFrameAndWEMaterialSlotIndex].Get() );
						writer.WriteString( t.entityelement[ent].eleprof.WEMaterial.normalMapName[iFrameAndWEMaterialSlotIndex].Get() );
						writer.WriteString( t.entityelement[ent].eleprof.WEMaterial.surfaceMapName[iFrameAndWEMaterialSlotIndex].Get() );
						writer.WriteString( t.entityelement[ent].eleprof.WEMaterial.displacementMapName[iFrameAndWEMaterialSlotIndex].Get() );
						writer.WriteString( t.entityelement[ent].eleprof.WEMaterial.emissiveMapName[iFrameAndWEMaterialSlotIndex].Get() );
						#ifndef DISABLEOCCLUSIONMAP
						writer.WriteString ( t.entityelement[ent].eleprof.WEMaterial.occlusionMapName[iFrameAndWEMaterialSlotIndex].Get() );
						#else
						writer.WriteString ( "" );
						#endif
						writer.WriteFloat( t.entityelement[ent].eleprof.WEMaterial.fNormal[iFrameAndWEMaterialSlotIndex] );
						writer.WriteFloat( t.entityelement[ent].eleprof.WEMaterial.fRoughness[iFrameAndWEMaterialSlotIndex] );
						writer.WriteFloat( t.entityelement[ent].eleprof.WEMaterial.fMetallness[iFrameAndWEMaterialSlotIndex] );
						writer.WriteFloat( t.entityelement[ent].eleprof.WEMaterial.fEmissive[iFrameAndWEMaterialSlotIndex] );
						writer.WriteFloat( t.entityelement[ent].eleprof.WEMaterial.fAlphaRef[iFrameAndWEMaterialSlotIndex] );
					}
				}
				if (t.versionnumbersave >= 318)
				{
					writer.WriteFloat( t.entityelement[ent].eleprof.WEMaterial.fRenderOrderBias[0] );
					for (int i = 1; i < MAXMESHMATERIALS; i++)
					{
						writer.WriteFloat( t.entityelement[ent].eleprof.WEMaterial.fRenderOrderBias[i] );
					}
				}
				if (t.versionnumbersave >= 319)
				{
					//PE: No relation between ent and all groups information, so only store it under ent = 1
					extern int g_iUniqueGroupID;
					if (ent > 1)
					{
						writer.WriteLong( 0 );
						writer.WriteLong( 0 ); //PE: zero groups on other ent entrys.
					}
					else
					{
						writer.WriteLong( g_iUniqueGroupID );
						int iNumberOfGroups = MAXGROUPSLISTS;
						writer.WriteLong( iNumberOfGroups );
						for (int gi = 0; gi < iNumberOfGroups; gi++)
						{
							int iItemsInThisGroup = vEntityGroupList[gi].size( );
							writer.WriteLong( iItemsInThisGroup );
							for (int i = 0; i < iItemsInThisGroup; i++)
							{
								writer.WriteLong( vEntityGroupList[gi][i].iGroupID );
								writer.WriteLong( vEntityGroupList[gi][i].iParentGroupID );
								writer.WriteLong( vEntityGroupList[gi][i].e );
								writer.WriteFloat( vEntityGroupList[gi][i].x );
								writer.WriteFloat( vEntityGroupList[gi][i].y );
								writer.WriteFloat( vEntityGroupList[gi][i].z );
								writer.WriteFloat( vEntityGroupList[gi][i].quatAngle.x );
								writer.WriteFloat( vEntityGroupList[gi][i].quatAngle.y );
								writer.WriteFloat( vEntityGroupList[gi][i].quatAngle.z );
								writer.WriteFloat( vEntityGroupList[gi][i].quatAngle.w );
							}
						}
						// and save out group thumb images, as need these to identify parent groups
						extern int iEntityGroupListImage[MAXGROUPSLISTS];
						for (int gi = 0; gi < iNumberOfGroups; gi++)
						{
							int iImgIndex = iEntityGroupListImage[gi];
							if (bForCollectionELE == false)
							{
								char pGroupImgFilename[MAX_PATH];
								sprintf(pGroupImgFilename, "%sgroupimg%d.png", g.mysystem.levelBankTestMap_s.Get(), gi);
								if (FileExist(pGroupImgFilename) == 1) DeleteAFile(pGroupImgFilename);
								if (iEntityGroupListImage[gi] > 0)
								{
									if (ImageExist(iImgIndex) == 1)
									{
										// img value not important, only as reference to image file creating now for the loader
										SaveImage(pGroupImgFilename, iImgIndex);
									}
								}
							}
							writer.WriteLong(iImgIndex > 0 ? 1 : 0);
						}
					}
				}

				if (t.versionnumbersave >= 320)
				{
					writer.WriteLong( t.entityelement[ent].eleprof.newparticle.bParticle_Preview );
					writer.WriteLong( t.entityelement[ent].eleprof.newparticle.bParticle_Show_At_Start );
					writer.WriteLong( t.entityelement[ent].eleprof.newparticle.bParticle_Looping_Animation );
					writer.WriteLong( t.entityelement[ent].eleprof.newparticle.bParticle_Full_Screen );
					writer.WriteFloat( t.entityelement[ent].eleprof.newparticle.fParticle_Fullscreen_Duration );
					writer.WriteFloat( t.entityelement[ent].eleprof.newparticle.fParticle_Fullscreen_Fadein );
					writer.WriteFloat( t.entityelement[ent].eleprof.newparticle.fParticle_Fullscreen_Fadeout );
					writer.WriteString( t.entityelement[ent].eleprof.newparticle.Particle_Fullscreen_Transition.Get() );
					writer.WriteFloat( t.entityelement[ent].eleprof.newparticle.fParticle_Speed );
					writer.WriteFloat( t.entityelement[ent].eleprof.newparticle.fParticle_Opacity );

				}
				if (t.versionnumbersave >= 321)
				{
					writer.WriteString( t.entityelement[ent].eleprof.newparticle.emittername.Get() );
				}

				if (t.versionnumbersave >= 322)
				{
					writer.WriteFloat( t.entityelement[ent].fDecalSpeed );
					writer.WriteFloat( t.entityelement[ent].fDecalOpacity );
				}
				if (t.versionnumbersave >= 323)
				{
					writer.WriteLong( t.entityelement[ent].eleprof.iOverrideCollisionMode );
				}
				if (t.versionnumbersave >= 324)
				{
					writer.WriteFloat( t.entityelement[ent].eleprof.weapondamagemultiplier );
					writer.WriteFloat( t.entityelement[ent].eleprof.meleedamagemultiplier );
				}
				if (t.versionnumbersave >= 325)
				{
					writer.WriteLong( t.entityelement[ent].eleprof.iAffectedByGravity );
					writer.WriteLong( t.entityelement[ent].eleprof.iMoveSpeed );
					writer.WriteLong( t.entityelement[ent].eleprof.iTurnSpeed );
				}
				if (t.versionnumbersave >= 326)
				{
					writer.WriteLong( t.entityelement[ent].eleprof.light.offsetup );  //Store spot radius.
				}

				if (t.versionnumbersave >= 327)
				{
					writer.WriteString( t.entityelement[ent].eleprof.soundset5_s.Get() );
					writer.WriteString( t.entityelement[ent].eleprof.soundset6_s.Get() );
				}

				if (t.versionnumbersave >= 328)
				{
					writer.WriteLong( t.entityelement[ent].eleprof.iUseSoundVariants );
				}
				if (t.versionnumbersave >= 329)
				{
					writer.WriteFloat(t.entityelement[ent].quatmode);
					writer.WriteFloat(t.entityelement[ent].quatx);
					writer.WriteFloat(t.entityelement[ent].quaty);
					writer.WriteFloat(t.entityelement[ent].quatz);
					writer.WriteFloat(t.entityelement[ent].quatw);
				}
				if (t.versionnumbersave >= 330)
				{
					writer.WriteFloat(t.entityelement[ent].eleprof.bAutoFlatten);
				}
				if (t.versionnumbersave >= 331)
				{
					writer.WriteString(t.entityelement[ent].eleprof.overrideanimset_s.Get());
				}
				if (t.versionnumbersave >= 332)
				{
					writer.WriteLong(t.entityelement[ent].eleprof.iscollectable);
				}
				if (t.versionnumbersave >= 333)
				{
					writer.WriteLong(t.entityelement[ent].eleprof.iSwimSpeed);
				}
				if (t.versionnumbersave >= 334)
				{
					extern cstr sEntityGroupListName[MAXGROUPSLISTS];
					int iNumberOfGroups = MAXGROUPSLISTS;
					writer.WriteLong(iNumberOfGroups);
					for (int gi = 0; gi < iNumberOfGroups; gi++)
					{
						writer.WriteString(sEntityGroupListName[gi].Get());
					}
				}
				if (t.versionnumbersave >= 335)
				{
					writer.WriteLong(t.entityelement[ent].creationOfGroupID);
				}
				if (t.versionnumbersave >= 336)
				{
					writer.WriteLong(t.entityelement[ent].eleprof.light.fLightHasProbeX);
					writer.WriteLong(t.entityelement[ent].eleprof.light.fLightHasProbeY);
					writer.WriteLong(t.entityelement[ent].eleprof.light.fLightHasProbeZ);
				}
				if (t.versionnumbersave >= 337)
				{
					writer.WriteLong(t.entityelement[ent].iCanGoUnderwater);
				}
				if (t.versionnumbersave >= 338)
				{
					writer.WriteLong(t.entityelement[ent].eleprof.clipcapacity);
					writer.WriteLong(t.entityelement[ent].eleprof.weaponpropres1);
					writer.WriteLong(t.entityelement[ent].eleprof.weaponpropres2);
				}
				if (t.versionnumbersave >= 339)
				{
					writer.WriteLong(t.entityelement[ent].eleprof.WEMaterial.customShaderID);
					writer.WriteFloat(t.entityelement[ent].eleprof.WEMaterial.customShaderParam1);
					writer.WriteFloat(t.entityelement[ent].eleprof.WEMaterial.customShaderParam2);
					writer.WriteFloat(t.entityelement[ent].eleprof.WEMaterial.customShaderParam3);
					writer.WriteFloat(t.entityelement[ent].eleprof.WEMaterial.customShaderParam4);
					writer.WriteFloat(t.entityelement[ent].eleprof.WEMaterial.customShaderParam5);
					writer.WriteFloat(t.entityelement[ent].eleprof.WEMaterial.customShaderParam6);
					writer.WriteFloat(t.entityelement[ent].eleprof.WEMaterial.customShaderParam7);
					writer.WriteString(t.entityelement[ent].eleprof.explodable_decalname.Get());
				}
				if (t.versionnumbersave >= 340)
				{
					writer.WriteString(t.entityelement[ent].eleprof.WEMaterial.WPEffect.Get());
					//PE: Fillers.

					writer.WriteFloat(t.entityelement[ent].eleprof.light.fProbeBrightness);

					//writer.WriteFloat(0.0f);
					writer.WriteLong(t.entityelement[ent].iAllowBuletHole);
					//writer.WriteFloat(0.0f);
					writer.WriteLong(t.entityelement[ent].eleprof.iMaterialSoundIndex);
				

					writer.WriteFloat(0.0f);
					writer.WriteFloat(0.0f);
					writer.WriteLong(t.entityelement[ent].eleprof.systemwide_lua);
					writer.WriteLong(t.entityelement[ent].eleprof.isobjective_alwaysactive);
					writer.WriteLong(t.entityelement[ent].eleprof.isProjectGlobal);
					writer.WriteString("");
					writer.WriteString("");
					writer.WriteString("");
				}
				if (t.versionnumbersave >= 341)
				{
					writer.WriteLong(t.entityelement[ent].eleprof.bUseFPESettings);
				}
				if (t.versionnumbersave >= 342)
				{
					writer.WriteString(t.entityelement[ent].eleprof.soundset4a_s.Get());
				}
			}
		} 
		if ( pass == 0 ) writer.AllocateDataForWrite();
	} // pass loop

	// write data buffer to file, EntityWriter will clean itself up
	if ( FileExist(t.elementsfilename_s.Get()) == 1  )  DeleteAFile ( t.elementsfilename_s.Get() );
	OpenToWrite ( 1, t.elementsfilename_s.Get() );
	WriteData( 1, writer.GetData(), writer.GetDataSize() );
	CloseFile ( 1 );
}

void entity_savebank ( void )
{
	//  Scan entire entityelement, delete all entitybank entries not used
	if (  g.gcompilestandaloneexe == 0 && g.gpretestsavemode == 0 ) 
	{
		if (  g.entidmaster>0 ) 
		{
			Dim ( t.entitybankused,g.entidmaster  );
			for ( t.tttentid = 1 ; t.tttentid<= g.entidmaster; t.tttentid++ )
			{
				t.entitybankused[t.tttentid]=0;
			}
			for ( t.ttte = 1 ; t.ttte <= g.entityelementlist; t.ttte++ )
			{
				t.tttentid = t.entityelement[t.ttte].bankindex;
				if (  t.tttentid > 0 && t.tttentid <= g.entidmaster ) 
				{
					t.entitybankused[t.tttentid]=1;
				}
			}
			bool bEntErasedDueToGroupCheckIfStillBeingUsed = false;
			for ( t.tttentid = 1 ; t.tttentid <= g.entidmaster; t.tttentid++ )
			{
				if (  t.entitybankused[t.tttentid] == 0 ) 
				{
					// do not remove if a smart object if still being used in level
					if (t.entityprofile[t.tttentid].model_s == "group")
					{
						// what group ID is this smart object?
						cstr tmp = cstr("entitybank\\") + t.entitybank_s[t.tttentid];
						int iSmartObjectGroupIndex = GetGroupIndexFromName(tmp);
						if (iSmartObjectGroupIndex >= 0 && iSmartObjectGroupIndex < MAXGROUPSLISTS)
						{
							// scan to see if this smart object group is being used in the level
							bool bBeingUsed = false;
							if (vEntityGroupList[iSmartObjectGroupIndex].size() > 0)
							{
								int iUniqueGroupID = vEntityGroupList[iSmartObjectGroupIndex][0].iGroupID;
								for (int ee = 1; ee <= g.entityelementlist; ee++)
								{
									if (t.entityelement[ee].bankindex > 0)
									{
										if (t.entityelement[ee].y > -48000.0f) // original smart object elements are buried deep and cloned, they do not count as part of level!
										{
											int thisGroupID = t.entityelement[ee].creationOfGroupID;
											if (thisGroupID > 0 && thisGroupID == iUniqueGroupID)
											{
												bBeingUsed = true;
											}
										}
									}
								}
							}
							if (bBeingUsed == true)
							{
								// we keep the group entity parent in place to access smart object from left panel
								t.entitybankused[t.tttentid] = 1;
							}
							else
							{
								// all instnces of smart object use removed from level, so finally remove the hidden elements as no longer needed
								if (vEntityGroupList[iSmartObjectGroupIndex].size() > 0)
								{
									int iUniqueGroupID = vEntityGroupList[iSmartObjectGroupIndex][0].iGroupID;
									for (int ee = 1; ee <= g.entityelementlist; ee++)
									{
										if (t.entityelement[ee].bankindex > 0)
										{
											if (t.entityelement[ee].y <= -48000.0f)
											{
												int thisGroupID = t.entityelement[ee].creationOfGroupID;
												if (thisGroupID > 0 && thisGroupID == iUniqueGroupID)
												{
													// original smart object elements buried deep, no longer needed
													int entid = t.entityelement[ee].bankindex;
													if (entid > 0)
													{
														t.entitybankused[entid] = 0;
														bEntErasedDueToGroupCheckIfStillBeingUsed = true;
													}
													t.entityelement[ee].bankindex = 0;
													t.entityelement[ee].creationOfGroupID = -1;
												}
											}
										}
									}
								}

								// and finally remove from group list records
								vEntityGroupList[iSmartObjectGroupIndex].clear();
								sEntityGroupListName[iSmartObjectGroupIndex] = "";
							}
						}
					}
					else
					{
						// are they in the collection list (must keep parent even if no element using it right now)
						for (int c = 0; c < g_collectionList.size(); c++)
						{
							if (g_collectionList[c].iEntityID == t.tttentid)
							{
								// being used
								t.entitybankused[t.tttentid] = 1;
							}
						}
					}
				}
			}
			// however, put back if the object was NOT part of a smart object
			if (bEntErasedDueToGroupCheckIfStillBeingUsed == true)
			{
				for (t.tttentid = 1; t.tttentid <= g.entidmaster; t.tttentid++)
				{
					if (t.entitybankused[t.tttentid] == 0)
					{
						// can remove the entity associated with the smart object
						bool bIsASmartObjectChildPart = true;
						for (int eee = 1; eee <= g.entityelementlist; eee++)
						{
							if (t.entityelement[eee].bankindex == t.tttentid && t.entityelement[eee].creationOfGroupID == -1)
							{
								bIsASmartObjectChildPart = false;
								break;
							}
						}
						if (bIsASmartObjectChildPart == false)
						{
							// do not remove, it may be in a smart object but also in the level independently
							t.entitybankused[t.tttentid] = 1;
						}
					}
				}
			}
			for (t.tttentid = 1; t.tttentid <= g.entidmaster; t.tttentid++)
			{
				if (t.entitybankused[t.tttentid] == 0)
				{
					// some debug help
					char debug[MAX_PATH];
					sprintf(debug, "Removing parent object as no longer used: %d - %s", t.tttentid, t.entitybank_s[t.tttentid].Get());
					timestampactivity(0, debug);

					// free RLE data in profile
					ebe_freecubedata (t.tttentid);

					//  remove entity entry if not used when save FPM
					t.entitybank_s[t.tttentid] = "";
				}
			}
			//  shuffle to remove empty entries
			for ( t.tttentid = 1 ; t.tttentid <= g.entidmaster; t.tttentid++ )
			{
				//  not used to record where entities have been moved to
				t.entitybankused[t.tttentid]=0;
			}
			t.treadentid=1 ; t.tlargest=0;
			for ( t.tttentid = 1 ; t.tttentid<=  g.entidmaster; t.tttentid++ )
			{
				if (  t.treadentid <= g.entidmaster ) 
				{
					while (  t.entitybank_s[t.treadentid] == "" ) 
					{
						++t.treadentid ; if (  t.treadentid>g.entidmaster  )  break;
					}
					if (  t.treadentid <= g.entidmaster ) 
					{
						t.entitybank_s[t.tttentid] = t.entitybank_s[t.treadentid];
						t.entityprofileheader[t.tttentid]=t.entityprofileheader[t.treadentid];
						t.entityprofile[t.tttentid]=t.entityprofile[t.treadentid];
						if ( t.entityprofile[t.treadentid].ebe.pRLEData != NULL && t.tttentid < t.treadentid )
						{
							// if we are shifting an EBE entity into place
							// EBE entity parents can be saved shortly after, so ensure object is copied over
							t.sourceobj = g.entitybankoffset + t.tttentid;
							if ( ObjectExist ( t.sourceobj ) == 1 ) DeleteObject ( t.sourceobj );
							int iSourceObjBeingMoved = g.entitybankoffset + t.treadentid;
							if ( ObjectExist ( iSourceObjBeingMoved ) == 1 ) CloneObject ( t.sourceobj, iSourceObjBeingMoved );
							
							// this overrites regular saved EBE entities
							t.entitybank_s[t.tttentid] = cstr("EBE") + cstr(t.tttentid);
						}
						t.entityprofile[t.tttentid].ebe.dwRLESize=t.entityprofile[t.treadentid].ebe.dwRLESize;
						t.entityprofile[t.tttentid].ebe.pRLEData=t.entityprofile[t.treadentid].ebe.pRLEData;
						t.entityprofile[t.tttentid].ebe.dwMatRefCount=t.entityprofile[t.treadentid].ebe.dwMatRefCount;
						t.entityprofile[t.tttentid].ebe.iMatRef=t.entityprofile[t.treadentid].ebe.iMatRef;
						t.entityprofile[t.tttentid].ebe.dwTexRefCount=t.entityprofile[t.treadentid].ebe.dwTexRefCount;
						t.entityprofile[t.tttentid].ebe.pTexRef=t.entityprofile[t.treadentid].ebe.pTexRef;
						for ( t.tt = 0 ; t.tt <=  100 ; t.tt++ ) t.entitybodypart[t.tttentid][t.tt]=t.entitybodypart[t.treadentid][t.tt] ;
						for ( t.tt = 0 ; t.tt <=  g.animmax ; t.tt++ ) t.entityanim[t.tttentid][t.tt]=t.entityanim[t.treadentid][t.tt] ;
						for ( t.tt = 0 ; t.tt <=  g.footfallmax ; t.tt++ ) t.entityfootfall[t.tttentid][t.tt]=t.entityfootfall[t.treadentid][t.tt] ; 
						for ( t.tt = 0 ; t.tt <=  100 ; t.tt++ ) t.entitydecal_s[t.tttentid][t.tt]=t.entitydecal_s[t.treadentid][t.tt] ;
						for ( t.tt = 0 ; t.tt <=  100 ; t.tt++ ) t.entitydecal[t.tttentid][t.tt]=t.entitydecal[t.treadentid][t.tt] ;
						t.entitybankused[t.treadentid]=t.tttentid;
						t.tlargest=t.tttentid;
					}
					else
					{
						// wipe after end of shuffle
						t.entitybank_s[t.tttentid] = "";
						t.entityprofileheader[t.tttentid].desc_s = "";
						t.entityprofile[t.tttentid].ebe.dwRLESize = 0;
						t.entityprofile[t.tttentid].ebe.pRLEData = NULL;
						t.entityprofile[t.tttentid].ebe.dwMatRefCount = 0;
						t.entityprofile[t.tttentid].ebe.iMatRef = NULL;
						t.entityprofile[t.tttentid].ebe.dwTexRefCount = 0;
						t.entityprofile[t.tttentid].ebe.pTexRef = NULL;
					}
				}
				else
				{
					// wipe after end of shuffle
					t.entitybank_s[t.tttentid]="";
					t.entityprofileheader[t.tttentid].desc_s = "";
					t.entityprofile[t.tttentid].ebe.dwRLESize = 0;
					t.entityprofile[t.tttentid].ebe.pRLEData = NULL;
					t.entityprofile[t.tttentid].ebe.dwMatRefCount = 0;
					t.entityprofile[t.tttentid].ebe.iMatRef = NULL;
					t.entityprofile[t.tttentid].ebe.dwTexRefCount = 0;
					t.entityprofile[t.tttentid].ebe.pTexRef = NULL;
				}
				++t.treadentid;
			}
			//  update bank index numbers in entityelements
			for ( t.ttte = 1 ; t.ttte <= g.entityelementlist; t.ttte++ )
			{
				t.tttentid = t.entityelement[t.ttte].bankindex;
				if ( t.tttentid > 0 && t.tttentid <= g.entidmaster ) 
				{
					if (t.entityelement[t.ttte].bankindex != t.entitybankused[t.tttentid])
					{
						// some debug help
						char debug[MAX_PATH];
						int iNewEntID = t.entitybankused[t.tttentid];
						sprintf(debug, "Object element %d reassigned parent object '%s' from %d to %d", t.ttte, t.entitybank_s[iNewEntID].Get(), t.entityelement[t.ttte].bankindex, iNewEntID);
						timestampactivity(0, debug);
					}

					// new entity entry place index
					t.entityelement[t.ttte].bankindex = t.entitybankused[t.tttentid];
				}
			}
			UnDim (  t.entitybankused );

			// new list size
			if ( g.entidmaster != t.tlargest ) 
			{
				// new parent object list (and trigger a load)
				g.entidmaster = t.tlargest;
				t.entityorsegmententrieschanged = 1;

				// when list changes, output a copy of the parent object list for debugging
				char debug[MAX_PATH];
				sprintf(debug, "Parent Object List (%d):", g.entidmaster);
				timestampactivity(0, debug);
				for (int id = 1; id <= g.entidmaster; id++)
				{
					sprintf(debug, "%d - %s", id, t.entitybank_s[id].Get());
					timestampactivity(0, debug);
				}
			}
		}
	}

	//  Save entity bank
	cstr entitybank_s = g.mysystem.levelBankTestMap_s+"map.ent";
	if ( FileExist(entitybank_s.Get()) == 1 ) DeleteAFile ( entitybank_s.Get() );
	OpenToWrite ( 1, entitybank_s.Get() );
	WriteLong ( 1, g.entidmaster );
	if ( g.entidmaster>0 ) 
	{
		for ( t.entid = 1 ; t.entid <= g.entidmaster; t.entid++ )
		{
			WriteString ( 1, t.entitybank_s[t.entid].Get() );
		}
	}
	CloseFile ( 1 );
}

void entity_savebank_ebe ( void )
{
	// Empty EBEs from testmap folder
	// 190417 - oops, this is deleting perfectly needed textures from testmap folder
	// when it should leave textures alone that may belong to the level EBEs
	// so set a flag to protect textures when save
	cstr pStoreOld = GetDir(); SetDir ( g.mysystem.levelBankTestMap_s.Get() ); //"levelbank\\testmap\\" );
	mapfile_emptyebesfromtestmapfolder(true);
	SetDir ( pStoreOld.Get() );

	// now save all EBE to testmap folder
	for ( t.tttentid = 1 ; t.tttentid <= g.entidmaster; t.tttentid++ )
	{
		if ( strlen(t.entitybank_s[t.tttentid].Get()) > 0 )
		{
			if ( t.entityprofile[t.tttentid].ebe.dwRLESize > 0 )
			{
				// Save EBE to represent this creation in the level
				cStr tSaveFile = g.mysystem.levelBankTestMap_s + cstr("ebe") + cstr(t.tttentid) + cstr(".ebe");
				
				ebe_save_ebefile ( tSaveFile, t.tttentid );
			}
		}
	}
}

void entity_loadbank ( void )
{
	// 050416 - build a black list for parental control (used below)
	 // No blacklist as all content is pre-vetted and clean

	 extern bool bKeepWindowsResponding;
	 void EmptyMessages(void);
	 if (bKeepWindowsResponding)
		 EmptyMessages();

	//  If ent file exists
	t.filename_s=t.levelmapptah_s+"map.ent";
	if (  FileExist(t.filename_s.Get()) == 1 ) 
	{
		//  Destroy old entities
		entity_deletebank ( );

		if (bKeepWindowsResponding)
			EmptyMessages();

		//  Load entity bank
		OpenToRead (  1, cstr(t.levelmapptah_s+"map.ent").Get() );
		g.entidmaster = ReadLong ( 1 );
		if (  g.entidmaster>0 ) 
		{
			entity_validatearraysize ( );
			for ( t.entid = 1 ; t.entid<=  g.entidmaster; t.entid++ )
			{
				t.entitybank_s[t.entid] = ReadString ( 1 );
			}
		}
		CloseFile (  1 );

		// 050416 - blank out any entities which are blacklisted
		g_bBlackListRemovedSomeEntities = false;
		if ( g_pBlackList != NULL )
		{
			char pThisEntityFilename[512];
			for ( t.entid = 1 ; t.entid <= g.entidmaster; t.entid++ )
			{
				strcpy ( pThisEntityFilename, t.entitybank_s[t.entid].Get() );
				strlwr ( pThisEntityFilename );
				for ( int iBlackListIndex=0; iBlackListIndex<g_iBlackListMax; iBlackListIndex++ )
				{
					int iBlacklistEntityNameLength = strlen ( g_pBlackList[iBlackListIndex] );
					int iCompareEnd = strlen ( pThisEntityFilename ) - iBlacklistEntityNameLength;
					if ( strnicmp ( g_pBlackList[iBlackListIndex], pThisEntityFilename + iCompareEnd, iBlacklistEntityNameLength )==NULL )
					{
						// this entity has been banned by parents
						g_bBlackListRemovedSomeEntities = true;
						t.entitybank_s[t.entid] = "";
					}
				}
			}
		}

		// 260215 - Do a pre-scan to determine if any entities are missing
		if ( Len(t.editor.replacefilepresent_s.Get())>1 ) 
		{
			// clear replacement output array
			Dim2( t.replacements_s, 1000, 1 );
			for ( int n=0; n < 1000; n++ )
			{
				t.replacements_s[n][0]="";
				t.replacements_s[n][1]="";
			}

			// load all replacements in a table
			Dim( t.replacementsinput_s, 1000 );
			t.treplacementmax=0;
			if ( FileExist(t.editor.replacefilepresent_s.Get()) == 1 ) 
			{
				LoadArray ( t.editor.replacefilepresent_s.Get(), t.replacementsinput_s );
				for ( t.l = 0 ; t.l <= ArrayCount(t.replacementsinput_s); t.l++ )
				{
					t.tline_s = t.replacementsinput_s[t.l];
					if ( Len(t.tline_s.Get()) > 0 ) 
					{
						for ( t.n = 1 ; t.n<=  Len(t.tline_s.Get()); t.n++ )
						{
							if (  cstr(Mid(t.tline_s.Get(),t.n)) == "=" ) 
							{
								t.told_s=Left(t.tline_s.Get(),t.n-1);
								t.told2_s="";
								for ( t.nn = 1 ; t.nn<=  Len(t.told_s.Get()); t.nn++ )
								{
									t.told2_s=t.told2_s+Mid(t.told_s.Get(),t.nn);
									if ( (cstr(Mid(t.told_s.Get(),t.nn)) == "\\" && cstr(Mid(t.told_s.Get(),t.nn+1)) == "\\") || (cstr(Mid(t.told_s.Get(),t.nn)) == "/" && cstr(Mid(t.told_s.Get(),t.nn+1)) == "/" ) )
									{
										++t.nn;
									}
								}
								t.tnew_s=Right(t.tline_s.Get(),Len(t.tline_s.Get())-t.n);
								++t.treplacementmax;
								t.replacements_s[t.treplacementmax][0]=Lower(t.told2_s.Get());
								t.replacements_s[t.treplacementmax][1]=Lower(t.tnew_s.Get());
							}
						}
					}
				}
			}

			if (bKeepWindowsResponding)
				EmptyMessages();

			//  go through all entities FPM is about to use
			for ( t.entid = 1 ; t.entid <= g.entidmaster; t.entid++ )
			{
				if ( strlen ( t.entitybank_s[t.entid].Get() ) > 0 )
				{
					LPSTR pEntityRef = t.entitybank_s[t.entid].Get();
					if ( pEntityRef[1] == ':')
					{
						// corrects for an ugly bug which exported entity bank references in absolute paths!
						t.tfile_s = t.entitybank_s[t.entid];
						for (t.tt = 1; t.tt <= t.treplacementmax; t.tt++)
						{
							LPSTR pReplaceSearch = t.replacements_s[t.tt][0].Get();
							LPSTR pThisEntFile = t.tfile_s.Get();
							if ( stricmp ( pReplaceSearch, pThisEntFile ) == NULL )
							{
								// found a match with an entry in the .REPLACE file
								// so replace master entry (entity instances will continue to reference by index)
								t.tnewent_s = t.replacements_s[t.tt][1];
								t.entitybank_s[t.entid] = Right(t.tnewent_s.Get(), Len(t.tnewent_s.Get()) - Len("entitybank\\"));
								break;
							}
						}
					}
					else
					{
						t.tfile_s = g.fpscrootdir_s + "\\Files\\entitybank\\" + t.entitybank_s[t.entid];
						if (FileExist(t.tfile_s.Get()) == 0)
						{
							t.tbinfile_s = cstr(Left(t.tfile_s.Get(), Len(t.tfile_s.Get()) - 4)) + cstr(".bin");
							if (FileExist(t.tbinfile_s.Get()) == 0)
							{
								//  cannot find FPE or BIN, so search replace file if we have a substitute
								t.tent_s = cstr("entitybank\\") + t.entitybank_s[t.entid];
								t.ttry2_s = "";
								for (t.nn = 1; t.nn <= Len(t.tent_s.Get()); t.nn++)
								{
									t.ttry2_s = t.ttry2_s + Mid(t.tent_s.Get(), t.nn);
									if ((cstr(Mid(t.tent_s.Get(), t.nn)) == "\\" && cstr(Mid(t.tent_s.Get(), t.nn + 1)) == "\\") || (cstr(Mid(t.tent_s.Get(), t.nn)) == "/" && cstr(Mid(t.tent_s.Get(), t.nn + 1)) == "/"))
									{
										++t.nn;
									}
								}
								t.tent_s = Lower(t.ttry2_s.Get());
								t.tfoundmatch = 0;
								for (t.tt = 1; t.tt <= t.treplacementmax; t.tt++)
								{
									if (t.replacements_s[t.tt][0] == t.tent_s)
									{
										//  found a match with an entry in the .REPLACE file
										//  so replace master entry (entity instances will continue to reference by index)
										t.tnewent_s = t.replacements_s[t.tt][1];
										t.entitybank_s[t.entid] = Right(t.tnewent_s.Get(), Len(t.tnewent_s.Get()) - Len("entitybank\\"));
										t.tfoundmatch = 1;
									}
								}
							}
						}
					}
				}
			}
			//  retain replacements$() for later in loadelementsdata
		}

		if (bKeepWindowsResponding)
			EmptyMessages();

		//  Load in all entity objects and data
		entity_loadentitiesnow ( );
	}
}

// allows extra entity to be loaded
int g_iAddEntitiesMode = 0;
int g_iAddEntitiesModeFrom = 0;

void entity_loadentitiesnow ( void )
{
	// Load entities specified by bank
	if ( g.entidmaster>0 ) 
	{
		char debug[MAX_PATH];
		sprintf(debug, "Loading master objects: %ld", g.entidmaster);
		extern int total_mem_from_load;
		total_mem_from_load = 0;
		timestampactivity(0, debug);
		int iFrom = 1;
		if (g_iAddEntitiesMode > 0) iFrom = g_iAddEntitiesModeFrom;
		for ( t.entid = iFrom; t.entid <= g.entidmaster; t.entid++ )
		{
			extern bool bKeepWindowsResponding;
			void EmptyMessages(void);
			if (bKeepWindowsResponding)
				EmptyMessages();

			// set entity name and load it in
			t.entdir_s = "entitybank\\";
			t.ent_s = t.entitybank_s[t.entid];
			t.entpath_s = getpath(t.ent_s.Get());
			t.tonscreenprompt_s = "";
			// if not an FPE, load FPE from EBE source
			if ( strcmp ( Lower(Right(t.ent_s.Get(),4)), ".fpe" ) != NULL )
			{
				// special EBE entity
				ebe_load_ebefile ( g.mysystem.levelBankTestMap_s + cstr("ebe") + cstr(t.entid) + cstr(".ebe"), t.entid );
				t.entityprofileheader[t.entid].desc_s = cstr("EBE") + cstr(t.entid);
				t.entdir_s = g.mysystem.levelBankTestMap_s;
				t.ent_s = cstr("ebe") + cstr(t.entid) + cstr(".fpe");
				t.entpath_s = "";
			}

			// when loading entities, all instances already in place, just need to create a STUB for the smart object
			extern int g_iAbortedAsEntityIsGroupFileMode;
			g_iAbortedAsEntityIsGroupFileMode = 1;

			// stubs will load the group, and that groups entities, but not instantiate the elements of that smart object (need ALL entities profiles to load in first!)
			extern int g_iAbortedAsEntityIsGroupFileModeStubOnly;
			g_iAbortedAsEntityIsGroupFileModeStubOnly = 1;

			extern uint32_t SetMasterObject;
			SetMasterObject = g.entitybankoffset + t.entid;
			// regular FPE entity
			entity_load ( );
			SetMasterObject = 0;

			// only used for when loading entities
			g_iAbortedAsEntityIsGroupFileModeStubOnly = 0;

			if ( t.game.runasmultiplayer == 1 ) mp_refresh ( );
			if (  t.desc_s == "" ) 
			{
				// free RLE data in profile
				ebe_freecubedata ( t.entid );
			
				//  where entities have been lost, delete from list
				t.entitybank_s[t.entid]="";
			}

			// keep multiplayer alive
			if ( t.game.runasmultiplayer == 1 ) mp_refresh ( );
		}
		timestampactivity(0, "End Loading master objects!");
	}
}

void entity_deletebank ( void )
{
	//  Destroy old entities
	if ( g.entidmastermax>0 ) 
	{
		for ( t.entid = 1 ; t.entid<=  g.entidmastermax; t.entid++ )
		{
			// delete parent entity object
			t.entobj = g.entitybankoffset+t.entid;
			if ( ObjectExist(t.entobj) == 1  ) DeleteObject (  t.entobj );

			//PE: Delete all textures used by master object here.
			//PE: TODO Perhaps use the .lst file from newly loaded level and do not delete those if they are used in the new level.
			void WickedCall_FreeImage_By_MasterID(uint32_t masterid);
			WickedCall_FreeImage_By_MasterID(t.entobj);

			// free RLE data in profile
			ebe_freecubedata ( t.entid );

			// wipe from table
			t.entitybank_s[t.entid]="";
		}
	}

	//  reset character creator bankoffset
	t.characterkitcontrol.bankOffset = 0;
	t.characterkitcontrol.count = 0; // 150216 - Dave needs to learn how to clean up after himself!

	//  Destroy profile data
	UnDim (  t.entityprofile );
	#ifdef DEFAULTMASTERENTITY
	Dim(t.entityprofile, DEFAULTMASTERENTITY);
	g.entidmastermax = DEFAULTMASTERENTITY;
	#else
	Dim(t.entityprofile, 100);
	g.entidmastermax = 100;
	#endif
	g.entidmaster=0;
}

void entity_deleteelementsdata ( void )
{
	//  Free any old elements
	entity_deleteelements ( );

	//  Clear counter for new load
	g.entityelementlist=0;
	g.entityelementmax=0;
}

void entity_deleteelements ( void )
{
	//  Quick deletes
	if ( g.entityelementlist > 0 ) 
	{
		DeleteObjects (  g.entityviewstartobj+1, g.entityviewstartobj+g.entityelementlist );
	}
	if ( g.entityattachmentindex > 0 ) 
	{
		DeleteObjects (g.entityattachmentsoffset, g.entityattachmentsoffset + g.entityattachmentindex);
		DeleteObjects (g.entityattachments2offset, g.entityattachments2offset + g.entityattachmentindex);
	}

	//  set character creator offset back to 0 (which is a nice indicator that it is not in use also)
	t.characterkitcontrol.offset = 0;
}

void entity_assignentityparticletodecalelement ( void )
{
	if (  t.originatore>0 ) 
	{
		if (  t.entityelement[t.originatore].eleprof.particleoverride == 1 ) 
		{
			t.decalelement[t.d].particle=t.entityelement[t.originatore].eleprof.particle;
		}
	}
}

void entity_addentitytomap_core ( void )
{
	// called from _entity_addentitytomap and also _game_masterroot
	if ( t.gridentityoverwritemode == 0 )
	{
		// First see if we have a prfeference (when click object to cursor, ideally want it dropped back in same t.e
		t.tokay = 0;
		if (t.gridentitypreferelementindex > 0)
		{
			if (t.entityelement[t.gridentitypreferelementindex].maintype == 0)
			{
				t.tokay = t.gridentitypreferelementindex;
			}
			t.gridentitypreferelementindex = 0;
		}

		// Create new or use free entity element
		if (t.tokay == 0 && g.entityelementlist > 0)
		{
			for (t.e = 1; t.e <= g.entityelementlist; t.e++)
			{
				if (t.entityelement[t.e].maintype == 0)
				{
					t.tokay = t.e;
					//PE: If old object was part of group remove it, or if will get deleted on next load.
					t.entityelement[t.e].creationOfGroupID = -1;
					break;
				}
			}
		}

		if ( t.tokay == 0 ) 
		{
			++g.entityelementlist;
			t.e=g.entityelementlist;
			if ( g.entityelementlist>g.entityelementmax ) 
			{
				Dim ( t.storeentityelement,g.entityelementmax );
				for ( t.e = 1 ; t.e<=  g.entityelementmax; t.e++ )
				{
					t.storeentityelement[t.e]=t.entityelement[t.e];
				}
				UnDim (  t.entityelement );
				UnDim (  t.entityshadervar );
				g.entityelementmax +=10;
				Dim (  t.entityelement,g.entityelementmax );
				Dim2(  t.entityshadervar,g.entityelementmax, g.globalselectedshadermax  );
				for ( t.e = 1 ; t.e<=  g.entityelementmax-10; t.e++ )
				{
					t.entityelement[t.e]=t.storeentityelement[t.e];
				}
			}
		}
		else
		{
			t.e=t.tokay;
		}
	}
	else
	{
		// can force new entity into a specific slot (when undo entiy group delete)
		t.e = t.gridentityoverwritemode;
	}

	//PE: Any newly added object should be unlocked.
	if (t.gridentityoverwritemode == 0)
	{
		extern std::vector<sRubberBandType> vEntityLockedList;
		t.entityelement[t.e].editorlock = 0;
		for (int i = 0; i < vEntityLockedList.size(); i++)
		{
			if (vEntityLockedList[i].e == t.e)
			{
				vEntityLockedList.erase(vEntityLockedList.begin() + i);
				sObject* pObject;
				if (t.entityelement[t.e].obj > 0)
				{
					if (t.entityelement[t.e].obj < g_iObjectListCount)
					{
						pObject = g_ObjectList[t.entityelement[t.e].obj];
						if (pObject)
						{
							WickedCall_SetObjectRenderLayer(pObject, GGRENDERLAYERS_NORMAL);
							WickedCall_SetObjectHighlightRed(pObject, false);
						}
					}
				}

				break;
			}
		}
	}

	// noticed newly created can have old garbage in them, clear to be safe
	t.entityelement[t.e].collected = 0;

	//  Fill entity element details
	t.entityelement[t.e].editorfixed=t.gridentityeditorfixed;
	t.entityelement[t.e].maintype=t.entitymaintype;
	t.entityelement[t.e].bankindex=t.entitybankindex;
	t.entityelement[t.e].staticflag=t.gridentitystaticmode;
	t.entityelement[t.e].iHasParentIndex=t.gridentityhasparent;
	t.entityelement[t.e].x=t.gridentityposx_f;
	t.entityelement[t.e].z=t.gridentityposz_f;
	t.entityelement[t.e].y=t.gridentityposy_f;
	t.entityelement[t.e].rx=t.gridentityrotatex_f;
	t.entityelement[t.e].ry=t.gridentityrotatey_f;
	t.entityelement[t.e].rz=t.gridentityrotatez_f;
	t.entityelement[t.e].quatmode = t.gridentityrotatequatmode;
	t.entityelement[t.e].quatx = t.gridentityrotatequatx_f;
	t.entityelement[t.e].quaty = t.gridentityrotatequaty_f;
	t.entityelement[t.e].quatz = t.gridentityrotatequatz_f;
	t.entityelement[t.e].quatw = t.gridentityrotatequatw_f;
	t.entityelement[t.e].scalex=t.gridentityscalex_f-100;
	t.entityelement[t.e].scaley=t.gridentityscaley_f-100;
	t.entityelement[t.e].scalez=t.gridentityscalez_f-100;
	t.entityelement[t.e].eleprof=t.grideleprof;
	t.entityelement[t.e].eleprof.light.index=0;
	t.entityelement[t.e].soundset = 0;
	t.entityelement[t.e].soundset1 = 0;
	t.entityelement[t.e].soundset2 = 0;
	t.entityelement[t.e].soundset3 = 0;
	t.entityelement[t.e].soundset4 = 0;
	t.entityelement[t.e].soundset5 = 0;
	t.entityelement[t.e].soundset6 = 0;
	t.entityelement[t.e].underground=0;
	t.entityelement[t.e].beenmoved=1;

	//PE: Always false by default.
	t.entityelement[t.e].eleprof.systemwide_lua = false;
	
	// auto flatten system
	t.entityelement[t.e].eleprof.iFlattenID = -1; // cannot carry this ID over
	if (!g_bEnableAutoFlattenSystem) //PE: If disabled always disable autoflatten.
		t.entityelement[t.e].eleprof.bAutoFlatten = false;
	extern bool g_bCreatingHiddenGroupInstance;
	if (g_bCreatingHiddenGroupInstance == true)
	{
		t.entityelement[t.e].iIsSmarkobjectDummyObj = 1;
	}

	t.entityelement[t.e].lua.outofrangefreeze = 0;
}

void entity_addentitytomap ( void )
{
	// Entity To Add
	t.entitymaintype=1;
	t.entitybankindex=t.gridentity;
	entity_addentitytomap_core ( );

	// transfer waypoint zone index to entityelement
	if (t.grideleprof.trigger.waypointzoneindex > 0)
	{
		if (t.grideleprof.trigger.waypointzoneindex < t.waypoint.size())
		{
			t.waypointindex = t.grideleprof.trigger.waypointzoneindex;
			t.entityelement[t.e].eleprof.trigger.waypointzoneindex = t.waypointindex;
			t.waypoint[t.waypointindex].linkedtoentityindex = t.e;
			waypoint_fixcorruptduplicate(t.e); // moved from below to here as we have valid t.waypointindex
		}
		t.grideleprof.trigger.waypointzoneindex = 0;
	}

	//  as create entity, apply any texture change required
	t.stentid=t.entid ; t.entid=t.entitybankindex;
	t.entdir_s="entitybank\\" ; t.ent_s=t.entitybank_s[t.entid] ; t.entpath_s=getpath(t.ent_s.Get());

	//  GRIDELEPROF might contain GUN+FLAK Data
	t.entid=t.entityelement[t.e].bankindex;
	t.tgunid_s=t.entityprofile[t.entid].isweapon_s;
	entity_getgunidandflakid ( );
	if (  t.tgunid>0 ) 
	{
		// populate the actual gun and flak settings (for further weapon entity creations)
		int firemode = 0; // 110718 - entity properties should only edit first primary gun settings (so we dont mess up enhanced weapons)
		g.firemodes[t.tgunid][firemode].settings.damage=t.grideleprof.damage;
		g.firemodes[t.tgunid][firemode].settings.accuracy=t.grideleprof.accuracy;
		g.firemodes[t.tgunid][firemode].settings.reloadqty=t.grideleprof.reloadqty;
		g.firemodes[t.tgunid][firemode].settings.iterate=t.grideleprof.fireiterations;
		g.firemodes[t.tgunid][firemode].settings.range=t.grideleprof.range;
		g.firemodes[t.tgunid][firemode].settings.dropoff=t.grideleprof.dropoff;
		g.firemodes[t.tgunid][firemode].settings.usespotlighting=t.grideleprof.usespotlighting;
		g.firemodes[t.tgunid][firemode].settings.clipcapacity = t.grideleprof.clipcapacity;
		g.firemodes[t.tgunid][firemode].settings.weaponpropres1 = t.grideleprof.weaponpropres1;
		g.firemodes[t.tgunid][firemode].settings.weaponpropres2 = t.grideleprof.weaponpropres2;

		//  which must also populate ALL other entities of same weapon
		t.tgunidchanged=t.tgunid;
		for ( t.te = 1 ; t.te<=  g.entityelementlist; t.te++ )
		{
			t.tentid=t.entityelement[t.te].bankindex;
			t.tgunid_s=t.entityprofile[t.tentid].isweapon_s;
			entity_getgunidandflakid ( );
			if (  t.tgunid == t.tgunidchanged ) 
			{
				t.entityelement[t.te].eleprof.damage=t.grideleprof.damage;
				t.entityelement[t.te].eleprof.accuracy=t.grideleprof.accuracy;
				t.entityelement[t.te].eleprof.reloadqty=t.grideleprof.reloadqty;
				t.entityelement[t.te].eleprof.fireiterations=t.grideleprof.fireiterations;
				t.entityelement[t.te].eleprof.range=t.grideleprof.range;
				t.entityelement[t.te].eleprof.dropoff=t.grideleprof.dropoff;
				t.entityelement[t.te].eleprof.usespotlighting=t.grideleprof.usespotlighting;
				t.entityelement[t.te].eleprof.clipcapacity = t.grideleprof.clipcapacity;
				t.entityelement[t.te].eleprof.weaponpropres1 = t.grideleprof.weaponpropres1;
				t.entityelement[t.te].eleprof.weaponpropres2 = t.grideleprof.weaponpropres2;
				t.entityelement[t.te].eleprof.lifespan=t.grideleprof.lifespan;
				t.entityelement[t.te].eleprof.throwspeed=t.grideleprof.throwspeed;
				t.entityelement[t.te].eleprof.throwangle=t.grideleprof.throwangle;
				t.entityelement[t.te].eleprof.bounceqty=t.grideleprof.bounceqty;
				t.entityelement[t.te].eleprof.explodeonhit=t.grideleprof.explodeonhit;
			}
		}
	}

	//  If multiplayer start marker, must propogate any script change to all others
	if (  t.entityprofile[t.entid].ismarker == 7 ) 
	{
		for ( t.te = 1 ; t.te<=  g.entityelementlist; t.te++ )
		{
			if (  t.te != t.e ) 
			{
				t.tentid=t.entityelement[t.te].bankindex;
				if (  t.entityprofile[t.tentid].ismarker == 7 ) 
				{
					t.entityelement[t.te].eleprof.aimain_s=t.entityelement[t.e].eleprof.aimain_s;
					t.entityelement[t.te].eleprof.soundset_s = t.entityelement[t.e].eleprof.soundset_s;
					t.entityelement[t.te].eleprof.soundset1_s = t.entityelement[t.e].eleprof.soundset1_s;
					t.entityelement[t.te].eleprof.soundset2_s = t.entityelement[t.e].eleprof.soundset2_s;
					t.entityelement[t.te].eleprof.soundset3_s = t.entityelement[t.e].eleprof.soundset3_s;
					t.entityelement[t.te].eleprof.soundset4_s = t.entityelement[t.e].eleprof.soundset4_s;
					t.entityelement[t.te].eleprof.soundset5_s = t.entityelement[t.e].eleprof.soundset5_s;
					t.entityelement[t.te].eleprof.soundset6_s = t.entityelement[t.e].eleprof.soundset6_s;
					t.entityelement[t.te].eleprof.soundset4a_s = t.entityelement[t.e].eleprof.soundset4a_s;
					t.entityelement[t.te].eleprof.overrideanimset_s = t.entityelement[t.e].eleprof.overrideanimset_s;
					t.entityelement[t.te].eleprof.strength=t.entityelement[t.e].eleprof.strength;
					t.entityelement[t.te].eleprof.speed=t.entityelement[t.e].eleprof.speed;
					t.entityelement[t.te].eleprof.animspeed=t.entityelement[t.e].eleprof.animspeed;
				}
			}
		}
	}

	//  Add entity reference into map
	t.tupdatee=t.e; entity_updateentityobj ( );

	// mark as static if it was
	if ( t.entityelement[t.tupdatee].staticflag == 1 ) g.projectmodifiedstatic = 1;

	// 160616 - just added EBE Builder New Site Entity 
	if ( t.entityprofile[t.gridentity].isebe > 0 )
	{
		if ( stricmp ( t.entitybank_s[t.gridentity].Get(), "..\\ebebank\\_builder\\New Site.fpe" ) == NULL )
		{
			// NEW FPE and EBE SITE
			t.entitybankindex = t.gridentity;

			// add new entity to entity library
			t.entityprofileheader[t.entitybankindex].desc_s = cstr("EBE") + cstr(t.entitybankindex);

			// update entityelement with new parent entity details
			t.entityelement[t.e].bankindex = t.entitybankindex;

			// change to site name after above creation from orig FPE template
			ebe_newsite ( t.e );

			// update entity FPE parent object from above newsite entity element obj
			t.entitybank_s[t.entitybankindex] = t.entityprofileheader[t.entitybankindex].desc_s;
			t.entityelement[t.e].bankindex = t.entitybankindex;
			t.entityelement[t.e].eleprof.name_s = t.entityprofileheader[t.entitybankindex].desc_s;
			ebe_updateparent ( t.e );

			// ensure mouse is released before painting on grid
			t.ebe.bReleaseMouseFirst = true;
		}
		else
		{
			// Selected an existing EBE entity from library
		}
	}

	//PE: Moved here as we use the object direction vector for spot lights.
	// update infinilight list with addition
	if (t.entityprofile[t.entitybankindex].ismarker == 2 || t.entityprofile[t.entitybankindex].ismarker == 5 || t.entityelement[t.e].eleprof.usespotlighting)
	{
		//PE: Some weapons have usespotlighting , this ruin all the lua light states.
		if (!(t.bSpawnCalledFromLua && t.entityprofile[t.entitybankindex].ismarker == 0 ))
		{
			lighting_refresh();
			entity_updatelightobj(t.e, t.entityelement[t.e].obj);
		}
	}

	// if new particle emitter, update it when created (to start the particle emission)
	entity_updateparticleemitter(t.tupdatee);

	// when add an entity to the scene, auto flatten if flagged
	if (t.entityelement[t.e].eleprof.iFlattenID == -1)
		entity_autoFlattenWhenAdded(t.e); //MD: Wrapped this section into a function to use when loading in levels to auto flatten areas
	else
		entity_updateautoflatten(t.e); //LB: Fix in case already added (in prepareobj for example)

	// ensure collection list up to date with new entity additions (such as weapons and other implied collectables)
	extern bool g_bSelectedNewObjectToAddToLevel;
	if (g_bSelectedNewObjectToAddToLevel == true)
	{
		g_bSelectedNewObjectToAddToLevel = false;
		extern bool g_bUpdateCollectionList;
		g_bUpdateCollectionList = true;
	}
}
bool bUpdateObjectList = false;
void entity_deleteentityfrommap ( void )
{
	bUpdateObjectList = true;
	//  Entity Type To Delete
	t.entitymaintype=1;

	//  Use entity coord to find tile
	t.tupdatee=t.tentitytoselect;

	// remember entity bank index for later light refresh
	int iWasEntID = t.entityelement[t.tupdatee].bankindex;

	// mark as static if it was
	if ( t.entityelement[t.tupdatee].staticflag == 1 ) g.projectmodifiedstatic = 1;

	//  blank from entity element list
	t.entityelement[t.tupdatee].bankindex=0;
	t.entityelement[t.tupdatee].maintype=0;
	t.entityelement[t.tupdatee].iHasParentIndex = 0;
	deleteinternalsound(t.entityelement[t.tupdatee].soundset) ; t.entityelement[t.tupdatee].soundset = 0;
	deleteinternalsound(t.entityelement[t.tupdatee].soundset1) ; t.entityelement[t.tupdatee].soundset1 = 0;
	deleteinternalsound(t.entityelement[t.tupdatee].soundset2) ; t.entityelement[t.tupdatee].soundset2 = 0;
	deleteinternalsound(t.entityelement[t.tupdatee].soundset3); t.entityelement[t.tupdatee].soundset3 = 0;
	deleteinternalsound(t.entityelement[t.tupdatee].soundset4); t.entityelement[t.tupdatee].soundset4 = 0;
	deleteinternalsound(t.entityelement[t.tupdatee].soundset5); t.entityelement[t.tupdatee].soundset5 = 0;
	deleteinternalsound(t.entityelement[t.tupdatee].soundset6); t.entityelement[t.tupdatee].soundset6 = 0;

	//  Delete any associated waypoint/trigger zone
	t.waypointindex=t.entityelement[t.tupdatee].eleprof.trigger.waypointzoneindex;
	if (  t.waypointindex > 0 ) 
	{
		if (  t.grideleprof.trigger.waypointzoneindex != t.waypointindex && t.tDontDeleteWPFlag  ==  0 ) 
		{
			t.w=t.waypoint[t.waypointindex].start;
			waypoint_delete ( );
		}
		t.entityelement[t.tupdatee].eleprof.trigger.waypointzoneindex=0;
	}
	t.tDontDeleteWPFlag = 0;

	// update infinilight list with removal
	if ( t.entityprofile[iWasEntID].ismarker == 2 || t.entityprofile[iWasEntID].ismarker == 5 ) 
	{
		//  refresh existing lights
		lighting_refresh ( );
	}
	//Delete any particle effects.
	if (g_UndoSysObjectIsBeingMoved != true)
	{
		int iParticleEmitter = t.entityelement[t.tupdatee].eleprof.newparticle.emitterid;
		if (iParticleEmitter != -1)
		{
			gpup_deleteEffect(iParticleEmitter);
			t.entityelement[t.tupdatee].eleprof.newparticle.emitterid = -1;
		}
	}


	t.entityelement[t.tupdatee].eleprof.blendmode = 0;

	// remove flatten if any
	if (t.entityelement[t.tupdatee].eleprof.iFlattenID != -1)
	{
		GGTerrain_RemoveFlatArea(t.entityelement[t.tupdatee].eleprof.iFlattenID);
		t.entityelement[t.tupdatee].eleprof.iFlattenID = -1;
	}

	// If the object is actually being deleted (and not added to cursor) then we need to remove any references to this object from OTHER object visual logic connections
	if (g_UndoSysObjectIsBeingMoved != true)
	{
		void GetRelationshipObject(int iFindLinkID, int* piEntityID, int* piObj);
		constexpr int maxLogicConnections = 10;
		int deletedObjectLinkID = t.entityelement[t.tupdatee].eleprof.iObjectLinkID;
		for (int i = 0; i < maxLogicConnections; i++)
		{
			int otherObj = 0, otherEntID = 0;
			GetRelationshipObject(t.entityelement[t.tupdatee].eleprof.iObjectRelationships[i], &otherEntID, &otherObj);
			if (otherEntID > 0)
			{
				entityeleproftype& otherEnt = t.entityelement[otherEntID].eleprof;
				// Find the logic connection that the deleted entity has to another entity
				for (int j = 0; j < maxLogicConnections; j++)
				{
					// Delete the logic connection that the other object has to the object pending deletion
					if (otherEnt.iObjectRelationships[j] == deletedObjectLinkID)
					{
						otherEnt.iObjectRelationships[j] = 0;
						otherEnt.iObjectRelationshipsData[j] = 0;
						otherEnt.iObjectRelationshipsType[j] = 0;
						break;
					}
				}
			}
		}
	}

	// deleting and readding objects, MUST wipe out relational data!
	for (int i = 0; i < 10; i++)
	{
		t.entityelement[t.tupdatee].eleprof.iObjectLinkID = 0;
		t.entityelement[t.tupdatee].eleprof.iObjectRelationships[i] = 0;
		t.entityelement[t.tupdatee].eleprof.iObjectRelationshipsData[i] = 0;
		t.entityelement[t.tupdatee].eleprof.iObjectRelationshipsType[i] = 0;
	}

	// update real ent obj (.obj=0 inside)
	entity_updateentityobj ( );
}

// new improved system using master and event stacks
void entity_performtheundoaction (eUndoEventType eventtype, void* pEventData)
{
	// if event data
	if (pEventData)
	{
		// perform editor action to undo these events
		switch (eventtype)
		{
		case eUndoSys_Object_Add:
		{
			sUndoSysEventObjectAdd* pEvent = (sUndoSysEventObjectAdd*)pEventData;
			t.tentitytoselect = pEvent->iEntityElementIndex;
			entity_deleteentityfrommap();
			break;
		}
		case eUndoSys_Object_Delete:
		{
			sUndoSysEventObjectDelete* pEvent = (sUndoSysEventObjectDelete*)pEventData;
			int store = t.e;
			t.e = pEvent->e;

			//t.entid = t.gridentity;
			t.entid = pEvent->entitybankindex;
			t.gridentity = t.entid;

			//PE: Before adding to map , make sure to fillout all informations from master to t.grideleprof.
			entity_fillgrideleproffromprofile();

			t.grideleprof.trigger.waypointzoneindex = pEvent->grideleprof_trigger_waypointzoneindex;
			t.entitybankindex = t.entid;
			t.gridentityeditorfixed = pEvent->gridentityeditorfixed;
			t.entitymaintype = pEvent->entitymaintype;
			t.entitybankindex = pEvent->entitybankindex;
			t.gridentitystaticmode = pEvent->gridentitystaticmode;
			t.gridentityhasparent = pEvent->gridentityhasparent;
			t.gridentityposx_f = pEvent->gridentityposx_f;
			t.gridentityposy_f = pEvent->gridentityposy_f;
			t.gridentityposz_f = pEvent->gridentityposz_f;
			t.gridentityrotatex_f = pEvent->gridentityrotatex_f;
			t.gridentityrotatey_f = pEvent->gridentityrotatey_f;
			t.gridentityrotatez_f = pEvent->gridentityrotatez_f;
			t.gridentityrotatequatmode = pEvent->gridentityrotatequatmode;
			t.gridentityrotatequatx_f = pEvent->gridentityrotatequatx_f;
			t.gridentityrotatequaty_f = pEvent->gridentityrotatequaty_f;
			t.gridentityrotatequatz_f = pEvent->gridentityrotatequatz_f;
			t.gridentityrotatequatw_f = pEvent->gridentityrotatequatw_f;
			t.gridentityscalex_f = pEvent->gridentityscalex_f + 100;
			t.gridentityscaley_f = pEvent->gridentityscaley_f + 100;
			t.gridentityscalez_f = pEvent->gridentityscalez_f + 100;
			t.gridentityoverwritemode = t.e;
			entity_addentitytomap ();
			t.gridentityoverwritemode = 0;
			t.gridentity = 0;
			t.e = store;
			break;
		}
		case eUndoSys_Object_ChangePosRotScl:
		{
			sUndoSysEventObjectChangePosRotScl* pEvent = (sUndoSysEventObjectChangePosRotScl*)pEventData;
			int te = pEvent->e;
			t.entityelement[te].x = pEvent->posx_f;
			t.entityelement[te].y = pEvent->posy_f;
			t.entityelement[te].z = pEvent->posz_f;
			t.entityelement[te].rx = pEvent->rotatex_f;
			t.entityelement[te].ry = pEvent->rotatey_f;
			t.entityelement[te].rz = pEvent->rotatez_f;		
			t.entityelement[te].quatmode = pEvent->rotatequatmode;
			t.entityelement[te].quatx = pEvent->rotatequatx_f;
			t.entityelement[te].quaty = pEvent->rotatequaty_f;
			t.entityelement[te].quatz = pEvent->rotatequatz_f;
			t.entityelement[te].quatw = pEvent->rotatequatw_f;
			t.entityelement[te].scalex = pEvent->scalex_f;
			t.entityelement[te].scaley = pEvent->scaley_f;
			t.entityelement[te].scalez = pEvent->scalez_f;
			t.tobj = t.entityelement[te].obj;
			t.tte = te;
			entity_positionandscale();
			break;
		}
		case eUndoSys_Object_DeleteWaypoint:
		{
			sUndoSysEventObjectDeleteWaypoint* pEvent = (sUndoSysEventObjectDeleteWaypoint*)pEventData;
			waypoint_undoredo_add(pEvent->waypoint, pEvent->waypointcoords);
			break;
		}
		case eUndoSys_Object_Group:
		{
			sUndoSysEventObjectGroup* pEvent = (sUndoSysEventObjectGroup*)pEventData;

			extern void UnGroupUndoSys(int index);
			UnGroupUndoSys(pEvent->groupindex);

			break;
		}
		case eUndoSys_Object_UnGroup:
		{
			sUndoSysEventObjectUnGroup* pEvent = (sUndoSysEventObjectUnGroup*)pEventData;

			extern void GroupUndoSys(int index, std::vector<sRubberBandType> groupData);
			GroupUndoSys(pEvent->groupindex, pEvent->groupData);
			
			break;
		}
		}

		// and delete the event data (the action consumes the memory created to store the event data)
		delete pEventData;
		pEventData = NULL;
	}
}
void entity_createundoaction (int eventtype, int te, bool bUserAction)
{
	// if user caused this action manually, must clear redo stack which 
	// contains events from previous alternative future
	if (bUserAction == true) undosys_clearredostack();

	// add specific event to stacks
	switch (eventtype)
	{
	case eUndoSys_Object_Add:
	{
		undosys_object_add(te);
		break;
	}
	case eUndoSys_Object_Delete:
	{
		undosys_object_delete (te, t.entityelement[te].eleprof.trigger.waypointzoneindex,
			t.entityelement[te].editorfixed,
			t.entityelement[te].maintype,
			t.entityelement[te].bankindex,
			t.entityelement[te].staticflag,
			t.entityelement[te].iHasParentIndex,
			t.entityelement[te].x,
			t.entityelement[te].y,
			t.entityelement[te].z,
			t.entityelement[te].rx,
			t.entityelement[te].ry,
			t.entityelement[te].rz,
			t.entityelement[te].quatmode,
			t.entityelement[te].quatx,
			t.entityelement[te].quaty,
			t.entityelement[te].quatz,
			t.entityelement[te].quatw,
			t.entityelement[te].scalex,
			t.entityelement[te].scaley,
			t.entityelement[te].scalez);
		break;
	}
	case eUndoSys_Object_ChangePosRotScl:
	{
		undosys_object_changeposrotscl (te, t.entityelement[te].x,
			t.entityelement[te].y,
			t.entityelement[te].z,
			t.entityelement[te].rx,
			t.entityelement[te].ry,
			t.entityelement[te].rz,
			t.entityelement[te].quatmode,
			t.entityelement[te].quatx,
			t.entityelement[te].quaty,
			t.entityelement[te].quatz,
			t.entityelement[te].quatw,
			t.entityelement[te].scalex,
			t.entityelement[te].scaley,
			t.entityelement[te].scalez);
		break;
	}
	case eUndoSys_Object_DeleteWaypoint:
	{
		undosys_object_deletewaypoint(t.entityelement[te].eleprof.trigger.waypointzoneindex, te);
		break;
	}
	case eUndoSys_Object_Group:
	{
		
		undosys_object_group(te);
		break;
	}
	case eUndoSys_Object_UnGroup:
	{
		extern std::vector<sRubberBandType> vEntityGroupList[MAXGROUPSLISTS];
		undosys_object_ungroup(te, vEntityGroupList[te]);
		break;
	}
	}
}
void entity_createtheredoaction (eUndoEventType eventtype, void* pEventData)
{
	switch (eventtype)
	{
	case eUndoSys_Object_Add: 
	{
		// about to undo this add (the _addundo will delete the object), so we create a delete event in the redo stack
		sUndoSysEventObjectAdd* pEvent = (sUndoSysEventObjectAdd*)pEventData;
		int te = pEvent->iEntityElementIndex;
		if (t.entityelement[te].eleprof.trigger.waypointzoneindex > 0)
		{
			undosys_multiplevents_start();
			entity_createundoaction(eUndoSys_Object_Delete, te, false);
			entity_createundoaction(eUndoSys_Object_DeleteWaypoint, te, false);
			undosys_multiplevents_finish();
		}
		else
		{
			entity_createundoaction(eUndoSys_Object_Delete, te, false);
		}
		break;
	}
	case eUndoSys_Object_Delete: 
	{
		// about to undo this delete (which the _deleteundo will add the object back in), so we create an add event in the redo stack
		sUndoSysEventObjectDelete* pEvent = (sUndoSysEventObjectDelete*)pEventData;
		int te = pEvent->e;
		entity_createundoaction(eUndoSys_Object_Add, te, false);
		break;
	}
	case eUndoSys_Object_ChangePosRotScl: 
	{
		// about to undo this move event (which will repos/rot/scl back to what was stored), so we create a move event in the redo stack to put it back the other way
		sUndoSysEventObjectChangePosRotScl* pEvent = (sUndoSysEventObjectChangePosRotScl*)pEventData;
		int te = pEvent->e;
		entity_createundoaction(eUndoSys_Object_ChangePosRotScl, te, false);
		break;
	}
	case eUndoSys_Object_DeleteWaypoint:
	{
		// About to undo a delete, so create an add event in the redo stack.
		sUndoSysEventObjectDeleteWaypoint* pEvent = (sUndoSysEventObjectDeleteWaypoint*)pEventData;
		int te = pEvent->e;
		entity_createundoaction(eUndoSys_Object_Add, te, false);
		break;
	}
	case eUndoSys_Object_Group:
	{
		sUndoSysEventObjectGroup* pEvent = (sUndoSysEventObjectGroup*)pEventData;
		int index = pEvent->groupindex;
		entity_createundoaction(eUndoSys_Object_UnGroup, index, false);
		break;
	}
	case eUndoSys_Object_UnGroup:
	{
		sUndoSysEventObjectUnGroup* pEvent = (sUndoSysEventObjectUnGroup*)pEventData;
		entity_createundoaction(eUndoSys_Object_Group, pEvent->groupindex, false);
		break;
	}
	}
}
void entity_undo (void)
{
	// undo system to control undo stack
	undosys_undoevent();
}
void entity_redo (void)
{
	// undo system to control redo stack
	undosys_redoevent();
}

std::vector<int> delete_decal_particles;
void delete_notused_decal_particles( void )
{
	//PE: Cleanup all decals.
	for (int a = 0; a < MAXUNIQUEDECALS; a++)
	{
		for (int i = 0; i < MAXREADYDECALS; i++)
		{
			if (ready_decals[a][i] > 0)
			{
				void DeleteEmitterEffects(uint32_t root);
				DeleteEmitterEffects(ready_decals[a][i]);
				ready_decals[a][i] = 0;
				decal_count[a] = 0;
			}
		}
	}
	for (int i = 1; i <= g.decalelementmax; i++)
	{
		if (t.decalelement[i].active == 1)
		{
			int did = t.decalelement[i].decalid;

			if (did >= t.decal.size())
				continue;

			if (t.decal[did].newparticle.bWPE)
			{
				if (t.decalelement[i].newparticle.emitterid > 0)
				{
					int iParticleEffect = t.decalelement[i].newparticle.emitterid;
					delete_decal_particles.erase(std::remove(delete_decal_particles.begin(), delete_decal_particles.end(), iParticleEffect), delete_decal_particles.end());
					void DeleteEmitterEffects(uint32_t root);
					DeleteEmitterEffects(iParticleEffect);
					t.decalelement[i].newparticle.emitterid = -1;
				}
			}
		}
	}
	for (int i = 0; i < delete_decal_particles.size(); i++)
	{
		gpup_deleteEffect(delete_decal_particles[i]);
	}
	//PE: Clear all deleted particles.
	delete_decal_particles.clear();
}

uint32_t WickedCall_LoadWiSceneDirect(Scene& scene2, char* filename, bool attached, char* changename, char* changenameto);
bool preload_wicked_particle_effect(newparticletype* pParticle, int decal_id)
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif

	// disable wicked particles (for testing/etc)
	extern int g_iDisableWParticleSystem;
	if (g_iDisableWParticleSystem == 1)
	{
		return false;
	}

	//PE: Preload effects so there is no delays.
	int MaxCachedDecals = MAXREADYDECALS;
	if (pParticle->iMaxCache > 0 && pParticle->iMaxCache < MAXREADYDECALS)
		MaxCachedDecals = pParticle->iMaxCache;

	int iParticleEmitter = pParticle->emitterid;
	if (iParticleEmitter == -1)
	{
		if (pParticle->bWPE)
		{
			Scene& scene = wiScene::GetScene();
			uint32_t master_root = 0;
			for (int i = 0; i < MaxCachedDecals; i++)
			{
				if (decal_id >= 1 && decal_id < MAXUNIQUEDECALS && ready_decals[decal_id][i] == 0)
				{
					uint32_t root = 0;
					Entity new_root = 0;
					uint32_t count_before = scene.emitters.GetCount();
					uint32_t mat_count_before = scene.materials.GetCount();

					char path[MAX_PATH];
					strcpy(path, pParticle->emittername.Get());
					GG_GetRealPath(path, 0);

					if (master_root > 0)
					{
						new_root = GetScene().Entity_Duplicate(master_root);
					}
					else
					{
						WickedCall_LoadWiScene(path, false, NULL, NULL);
					}

					uint32_t count_after = scene.emitters.GetCount();
					if (count_before != count_after)
					{
						Entity emitter = scene.emitters.GetEntity(scene.emitters.GetCount() - 1);
						Entity matemitter = scene.materials.GetEntity(scene.materials.GetCount() - 1);
						if (scene.emitters.GetCount() > 0)
						{
							HierarchyComponent* hier = scene.hierarchy.GetComponent(emitter);
							if (hier)
							{
								root = hier->parentID;
							}
						}
						wiEmittedParticle* ec = scene.emitters.GetComponent(emitter);
						if (ec)
						{
							//ec->Restart();
							ec->SetVisible(true);
						}

						if (master_root > 0)
						{
							//PE: resource sometimes empty when using Entity_Duplicate.
							int from = mat_count_before;
							int to = scene.materials.GetCount();
							for (; from < to; from++)
							{
								for (int a = 0; a < MaterialComponent::EMISSIVEMAP; a++)
								{
									if (scene.materials[from].textures[a].name.size() > 0)
									{
										if (!scene.materials[from].textures[a].resource)
										{
											scene.materials[from].textures[a].resource = WickedCall_LoadImage(scene.materials[from].textures[a].name);
										}
									}
								}
								scene.materials[from].SetDirty();
							}
						}

					}
					if (root != 0)
					{
						if (decal_id >= 1 && decal_id < MAXUNIQUEDECALS && ready_decals[decal_id][i] == 0)
						{
							iParticleEmitter = pParticle->emitterid = root;
							ready_decals[decal_id][i] = root;
						}
						else
						{
							iParticleEmitter = pParticle->emitterid = root;
						}
						if (master_root == 0)
						{
							master_root = root;
							//PE: Validate that decal is a burst only emitter.
							for (int b = 0; b < scene.emitters.GetCount(); b++)
							{
								Entity emitter = scene.emitters.GetEntity(b);
								HierarchyComponent* hier = scene.hierarchy.GetComponent(emitter);
								if (hier)
								{
									if (hier->parentID == master_root)
									{
										wiEmittedParticle* ec = scene.emitters.GetComponent(emitter);
										if (ec->count > 0.1f)
										{
											//PE: This is not a burst emitter reject it.
											iParticleEmitter = pParticle->emitterid = -1;
											ready_decals[decal_id][i] = 0;

											WickedCall_PerformEmitterAction(2, root); //PE: Pause
											WickedCall_PerformEmitterAction(6, root); //PE: Not Visible
											void DeleteEmitterEffects(uint32_t root);
											DeleteEmitterEffects(master_root);
											return false;
										}
									}
								}
							}
						}

						WickedCall_PerformEmitterAction(2, root); //PE: Pause
						WickedCall_PerformEmitterAction(6, root); //PE: Not Visible
					}
				}
			}
		}
	}
	return true;
}

void newparticle_updateparticleemitter ( newparticletype* pParticle, float fScale, float fX, float fY, float fZ, float fRX, float fRY, float fRZ, GGMATRIX* pmatBaseRotation,bool bAutoDelete,int decal_id)
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif

	// show or hide based on editor vs test game
	bool bShowThisParticle = false;
	extern bool bImGuiInTestGame;
	if (bImGuiInTestGame == true)
		bShowThisParticle = pParticle->bParticle_Show_At_Start;
	else
		bShowThisParticle = pParticle->bParticle_Preview;

	int iParticleEmitter = pParticle->emitterid;
	if (iParticleEmitter == -1)
	{
		if (pParticle->bWPE)
		{
			int MaxCachedDecals = MAXREADYDECALS;
			if (pParticle->iMaxCache > 0 && pParticle->iMaxCache < MAXREADYDECALS)
				MaxCachedDecals = pParticle->iMaxCache;

			Scene& scene = wiScene::GetScene();
			if (bShowThisParticle == true)
			{
				//PE: Use Cached effects.
				if (decal_id >= 1 && decal_id < MAXUNIQUEDECALS && ready_decals[decal_id][decal_count[decal_id]] > 0)
				{
					iParticleEmitter = pParticle->emitterid = ready_decals[decal_id][decal_count[decal_id]];
					decal_count[decal_id]++;
					if (decal_count[decal_id] >= MaxCachedDecals)
						decal_count[decal_id] = 0;
				}
				else
				{
					uint32_t root = 0;
					uint32_t count_before = scene.emitters.GetCount();

					char path[MAX_PATH];
					strcpy(path, pParticle->emittername.Get());
					GG_GetRealPath(path, 0);

					WickedCall_LoadWiScene(path, false, NULL, NULL);
					uint32_t count_after = scene.emitters.GetCount();
					if (count_before != count_after)
					{
						Entity emitter = scene.emitters.GetEntity(scene.emitters.GetCount() - 1);
						if (scene.emitters.GetCount() > 0)
						{
							HierarchyComponent* hier = scene.hierarchy.GetComponent(emitter);
							if (hier)
							{
								root = hier->parentID;
							}
						}
						wiEmittedParticle* ec = scene.emitters.GetComponent(emitter);
						if (ec)
						{
							ec->Restart();
							ec->SetVisible(true);
						}
					}
					if (root != 0)
					{
						if (decal_id >= 1 && decal_id < MAXUNIQUEDECALS && ready_decals[decal_id][decal_count[decal_id]] == 0)
						{
							iParticleEmitter = pParticle->emitterid = root;
							ready_decals[decal_id][decal_count[decal_id]] = root;
						}
						else
						{
							iParticleEmitter = pParticle->emitterid = root;
							if (bAutoDelete)
								delete_decal_particles.push_back(iParticleEmitter);
						}
						//PE: Not cached effects (not active at start) only use 2 cached versions.
						decal_count[decal_id]++;
						if (decal_count[decal_id] >= MaxCachedDecals)
							decal_count[decal_id] = 0;
					}
				}
			}
		}
		else
		if (bShowThisParticle == true)
		{
			iParticleEmitter = gpup_loadEffect(pParticle->emittername.Get(), 0, 0, 0, 1.0);
			gpup_emitterActive(iParticleEmitter, 0);
			pParticle->emitterid = iParticleEmitter;
			//PE: Anything created in testgame (LUA) must be deleted after game.
			if(bAutoDelete || bImGuiInTestGame == true)
				delete_decal_particles.push_back(iParticleEmitter);
		}
	}
	if (iParticleEmitter != -1)
	{
		/*
		if (pParticle->bWPE)
		{
			Scene& scene = wiScene::GetScene();
			TransformComponent* root_tranform = scene.transforms.GetComponent(iParticleEmitter);
			if (bShowThisParticle)
			{
				if (pParticle->bParticle_Fire == true)
				{
					if (root_tranform)
					{
						root_tranform->ClearTransform();
						root_tranform->Translate(XMFLOAT3(fX, fY, fZ));
						root_tranform->UpdateTransform();
					}
					WickedCall_PerformEmitterAction(3, iParticleEmitter); //PE: Resume
					WickedCall_PerformEmitterAction(4, iParticleEmitter); //PE: Restart
					WickedCall_PerformEmitterAction(5, iParticleEmitter); //PE: Visible
					WickedCall_PerformEmitterAction(1, iParticleEmitter); //PE: Burst All
					pParticle->bParticle_Fire = false;
				}
			}
		}
		*/

		//PE: Make emitter always face camera.
		if (pParticle->bWPE)
		{
			Scene& scene = wiScene::GetScene();
			TransformComponent* root_tranform = scene.transforms.GetComponent(iParticleEmitter);

			if (bShowThisParticle)
			{
				if (pParticle->bParticle_Fire == true)
				{
					if (root_tranform)
					{
						root_tranform->ClearTransform();
						XMVECTOR cameraPos = XMVectorSet(CameraPositionX(0), CameraPositionY(0), CameraPositionZ(0), 0.0f);
						XMVECTOR emitterPos = XMVectorSet(fX, fY, fZ, 0.0f);

						XMVECTOR lookDir = XMVectorSubtract(cameraPos, emitterPos);
						//PE: Invert drection - XMVECTOR lookDir = XMVectorSubtract(emitterPos, cameraPos);
						lookDir = XMVectorSetY(lookDir, 0.0f);

						if (XMVectorGetX(XMVector3LengthSq(lookDir)) > 0.0001f)
						{
							lookDir = XMVector3Normalize(lookDir);
							XMVECTOR worldUp = XMVectorSet(0, 1, 0, 0);
							XMMATRIX lookAtMat = XMMatrixLookToLH(emitterPos, lookDir, worldUp);
							XMMATRIX worldMat = XMMatrixInverse(nullptr, lookAtMat);
							root_tranform->MatrixTransform(worldMat);
						}
						else
						{
							root_tranform->Translate(XMFLOAT3(fX, fY, fZ));
						}

						root_tranform->UpdateTransform();
					}

					WickedCall_PerformEmitterAction(3, iParticleEmitter); // Resume
					WickedCall_PerformEmitterAction(4, iParticleEmitter); // Restart
					WickedCall_PerformEmitterAction(5, iParticleEmitter); // Visible
					WickedCall_PerformEmitterAction(1, iParticleEmitter); // Burst All
					pParticle->bParticle_Fire = false;
				}
			}
		}
		else
		{
			// set emitter position, rotation and scale
			gpup_setGlobalPosition(iParticleEmitter, fX, fY, fZ);
			if (pParticle->bParticle_Offset_Used == true)
			{
				float x = pParticle->bParticle_Offset_X;
				float y = pParticle->bParticle_Offset_Y;
				float z = pParticle->bParticle_Offset_Z;
				gpup_setLocalPosition(iParticleEmitter, x, y, z);
			}
			else
			{
				gpup_resetLocalPosition(iParticleEmitter);
			}
			float fSpeedX, fSpeedY, fSpeedZ;
			gpup_getEmitterSpeedAngleAdjustment(iParticleEmitter, &fSpeedX, &fSpeedY, &fSpeedZ);
			GGVECTOR3 vecSpeedDirection = GGVECTOR3(fSpeedX - 0.5f, fSpeedY - 0.5f, fSpeedZ - 0.5f);
			if (pParticle->bParticle_LocalRot_Used == true)
			{
				// local emitter rotation
				float x = GGToRadian(pParticle->bParticle_LocalRot_X);
				float y = GGToRadian(pParticle->bParticle_LocalRot_Y);
				float z = GGToRadian(pParticle->bParticle_LocalRot_Z);
				GGMATRIX matLocalRot;
				GGMatrixRotationYawPitchRoll(&matLocalRot, y, x, z);
				GGVec3TransformCoord(&vecSpeedDirection, &vecSpeedDirection, &matLocalRot);
				if (pmatBaseRotation)
				{
					// global rotation
					GGVec3TransformCoord(&vecSpeedDirection, &vecSpeedDirection, pmatBaseRotation);
				}
				gpup_setEmitterSpeedAngleAdjustment(iParticleEmitter, 0.5f + vecSpeedDirection.x, 0.5f + vecSpeedDirection.y, 0.5f + vecSpeedDirection.z);
			}
			gpup_setGlobalRotation(iParticleEmitter, fRX, fRY, fRZ);
			gpup_setGlobalScale(iParticleEmitter, 100.0f + fScale);

			// set whether burst mode loops
			if (pParticle->bParticle_Looping_Animation == true)
				gpup_emitterBurstMode(iParticleEmitter, 0);
			else
				gpup_emitterBurstMode(iParticleEmitter, 1);

			// switch emitter on or off
			if (bShowThisParticle == true)
			{
				gpup_emitterActive(iParticleEmitter, 1);

				// in a test game, drawn again if the test game's start hid it (newparticle_testgamebegin)
				if (bImGuiInTestGame == true && gpup_getEffectVisible(iParticleEmitter) == 0)
					gpup_setEffectVisible(iParticleEmitter, 1);
			}
			else
				gpup_emitterActive(iParticleEmitter, 0);

			// specify psrticle speed
			if (pParticle->bParticle_SpeedChange == true)
			{
				if (pParticle->fParticle_Speed_Original == -123.0f) pParticle->fParticle_Speed_Original = gpup_getParticleSpeed(iParticleEmitter);
				gpup_setEffectAnimationSpeed(iParticleEmitter, pParticle->fParticle_Speed);
			}
			else
			{
				if (pParticle->fParticle_Speed_Original != -123.0f)
				{
					gpup_setEffectAnimationSpeed(iParticleEmitter, pParticle->fParticle_Speed_Original);
				}
			}

			// specify psrticle opacity
			if (pParticle->bParticle_OpacityChange == true)
			{
				if (pParticle->fParticle_Opacity_Original == -123.0f) pParticle->fParticle_Opacity_Original = gpup_getParticleOpacity(iParticleEmitter);
				gpup_setEffectOpacity(iParticleEmitter, pParticle->fParticle_Opacity);
			}
			else
			{
				if (pParticle->fParticle_Opacity_Original != -123.0f)
				{
					gpup_setEffectOpacity(iParticleEmitter, pParticle->fParticle_Opacity_Original);
				}
			}

			// specify particle size
			if (pParticle->bParticle_SizeChange == true)
			{
				if (pParticle->bParticle_Size_Original == -123.0f) pParticle->bParticle_Size_Original = gpup_getParticleSize(iParticleEmitter);
				gpup_setParticleScale(iParticleEmitter, pParticle->bParticle_Size);
			}
			else
			{
				if (pParticle->bParticle_Size_Original != -123.0f)
				{
					gpup_setParticleScale(iParticleEmitter, pParticle->bParticle_Size_Original);
				}
			}

			// handle any triggering of a fire burst
			if (pParticle->bParticle_Fire == true)
			{
				gpup_emitterFire(iParticleEmitter);
				pParticle->bParticle_Fire = false;
			}

			// handle particle collisions with floor and sphere (for reflection bounce)
			if (pParticle->fParticle_BouncinessChange == true)
			{
				if (pParticle->fParticle_Bounciness_Original == -123.0f) pParticle->fParticle_Bounciness_Original = gpup_getBounciness(iParticleEmitter) * 5.0f;
				gpup_setBounciness(iParticleEmitter, pParticle->fParticle_Bounciness / 5.0f);
			}
			else
			{
				if (pParticle->fParticle_Bounciness_Original != -123.0f)
				{
					gpup_setBounciness(iParticleEmitter, pParticle->fParticle_Bounciness_Original / 5.0f);
				}
			}
			if (pParticle->iParticle_Floor_Active > 0)
			{
				if (pParticle->fParticle_Floor_Height_Original == -123.0f) pParticle->fParticle_Floor_Height_Original = gpup_getFloorReflectionHeight(iParticleEmitter);
				gpup_floorReflection(iParticleEmitter, pParticle->iParticle_Floor_Active - 1, pParticle->fParticle_Floor_Height);
			}
			else
			{
				if (pParticle->fParticle_Floor_Height_Original != -123.0f)
				{
					gpup_restoreFloorReflection(iParticleEmitter, 1, pParticle->fParticle_Floor_Height_Original);
				}
			}

			// handle color if particle effect
			if (pParticle->bParticle_ColorChange == true)
			{
				if (pParticle->fParticle_R_Original == -123.0f) gpup_getEffectColor(iParticleEmitter, &pParticle->fParticle_R_Original, &pParticle->fParticle_G_Original, &pParticle->fParticle_B_Original);
				gpup_setEffectColor(iParticleEmitter, pParticle->fParticle_R, pParticle->fParticle_G, pParticle->fParticle_B);
			}
			else
			{
				if (pParticle->fParticle_R_Original != -123.0f)
				{
					gpup_setEffectColor(iParticleEmitter, pParticle->fParticle_R_Original, pParticle->fParticle_G_Original, pParticle->fParticle_B_Original);
				}
			}

			// handle change of lifespan
			if (pParticle->bParticle_LifespanChange == true)
			{
				if (pParticle->fParticle_Lifespan_Original == -123.0f) pParticle->fParticle_Lifespan_Original = gpup_getEffectLifespan(iParticleEmitter);
				gpup_setEffectLifespan(iParticleEmitter, pParticle->fParticle_Lifespan);
			}
			else
			{
				if (pParticle->fParticle_Lifespan_Original != -123.0f)
				{
					gpup_setEffectLifespan(iParticleEmitter, pParticle->fParticle_Lifespan_Original);
				}
			}
		}
	}
}

// a test game starts with every legacy particle effect the editor made switched off and hidden, then each particle entity
// set as the game wants it (Show at start of level), so an effect previewed in the editor, or one no entity owns any more,
// does not burn on in the game; when the test game ends each is put back as it was
struct sParticleTestGameState
{
	int iID;
	int iActive;
	int iVisible;
};
std::vector<sParticleTestGameState> g_ParticleTestGameStates;
bool g_bParticleTestGameBegun = false;

void newparticle_testgamebegin ( void )
{
	if ( g_bParticleTestGameBegun == false )
	{
		g_ParticleTestGameStates.clear();
		for ( int iID = 0; iID < gpup_getMaxEffects(); iID++ )
		{
			if ( gpup_getEffectLoaded(iID) == 0 ) continue;
			sParticleTestGameState state;
			state.iID = iID;
			state.iActive = gpup_getEmitterActive(iID);
			state.iVisible = gpup_getEffectVisible(iID);
			g_ParticleTestGameStates.push_back(state);
			gpup_emitterActive(iID, 0);
			gpup_setEffectVisible(iID, 0);
		}
		g_bParticleTestGameBegun = true;
	}
	for ( int e = 1; e <= g.entityelementlist; e++ )
	{
		int entid = t.entityelement[e].bankindex;
		if ( entid > 0 && t.entityprofile[entid].ismarker == 10 )
		{
			entity_updateparticleemitter(e);
		}
	}
}

void newparticle_testgameend ( void )
{
	for ( int i = 0; i < (int)g_ParticleTestGameStates.size(); i++ )
	{
		int iID = g_ParticleTestGameStates[i].iID;
		if ( gpup_getEffectLoaded(iID) == 0 ) continue;
		gpup_emitterActive(iID, g_ParticleTestGameStates[i].iActive);
		gpup_setEffectVisible(iID, g_ParticleTestGameStates[i].iVisible);
	}
	g_ParticleTestGameStates.clear();
	g_bParticleTestGameBegun = false;
}

void newparticle_deleteparticleemitter( int iParticleEffect )
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif

	delete_decal_particles.erase(std::remove(delete_decal_particles.begin(), delete_decal_particles.end(), iParticleEffect), delete_decal_particles.end());
	gpup_deleteEffect(iParticleEffect);
}

void entity_updateparticleemitterbyID ( entityeleproftype* pEleprof, int iObj, float fScale, float fX, float fY, float fZ, float fRX, float fRY, float fRZ)
{
	// get base rotation of object (for speed vector control)
	GGMATRIX* pmatBaseRotation = NULL;
	sObject* pObject = GetObjectData(iObj);
	if (pObject) pmatBaseRotation = &pObject->position.matRotation;

	// control particle settings via ptr
	newparticle_updateparticleemitter(&pEleprof->newparticle, fScale, fX, fY, fZ, fRX, fRY, fRZ, pmatBaseRotation,false, -1);
}

void entity_updateparticleemitter ( int e )
{
	// also handle particle emitter entity
	if (e > 0)
	{
		if (t.entityprofile[t.entityelement[e].bankindex].ismarker == 10)
		{
			entity_updateparticleemitterbyID(&t.entityelement[e].eleprof, t.entityelement[e].obj, t.entityelement[e].scalex, t.entityelement[e].x, t.entityelement[e].y, t.entityelement[e].z, t.entityelement[e].rx, t.entityelement[e].ry, t.entityelement[e].rz);
		}
	}
}

void entity_updateautoflatten (int e, int obj)
{
	int entid = t.entityelement[e].bankindex;
	if (entid > 0)
	{
		int iAutoFlattenMode = t.entityprofile[entid].autoflatten;

		if ((iAutoFlattenMode != 0 && !g.isGameBeingPlayed) && (!g_bEnableAutoFlattenSystem || !t.entityelement[e].eleprof.bAutoFlatten) )
		{
			if (t.entityelement[e].eleprof.iFlattenID != -1)
			{
				//PE: Disabled remove any flatten.
				GGTerrain_RemoveFlatArea(t.entityelement[e].eleprof.iFlattenID);
				t.entityelement[e].eleprof.iFlattenID = -1;
				//t.entityelement[e].eleprof.bAutoFlatten = false;
			}
		}
		else if (iAutoFlattenMode != 0 && !g.isGameBeingPlayed)
		{
			//PE: t.entityelement[e].obj is not set in creating process. entity_prepareobj()
			int iObj = t.entityelement[e].obj;
			if (iObj == 0 && obj > 0) iObj = obj; //PE: entity_prepareobj() now set obj for standalone.
			if (iObj > 0)
			{
				float x = t.entityelement[e].x + (GetObjectCollisionCenterX(iObj) * (ObjectScaleX(iObj) / 100.0f));
				float y = t.entityelement[e].y + (GetObjectCollisionCenterY(iObj) * (ObjectScaleY(iObj) / 100.0f)) - (ObjectSizeY(iObj) / 2, 1);
				float z = t.entityelement[e].z + (GetObjectCollisionCenterZ(iObj) * (ObjectScaleZ(iObj) / 100.0f));

				GGQUATERNION QuatAroundX, QuatAroundY, QuatAroundZ, quatRotationEvent;
				GGQuaternionRotationAxis(&QuatAroundX, &GGVECTOR3(1, 0, 0), GGToRadian(ObjectAngleX(iObj)));
				GGQuaternionRotationAxis(&QuatAroundY, &GGVECTOR3(0, 1, 0), GGToRadian(ObjectAngleY(iObj)));
				GGQuaternionRotationAxis(&QuatAroundZ, &GGVECTOR3(0, 0, 1), GGToRadian(ObjectAngleZ(iObj)));
				quatRotationEvent = QuatAroundX * QuatAroundY * QuatAroundZ;
				
				float a = 2 * (quatRotationEvent.x * quatRotationEvent.z + quatRotationEvent.w * quatRotationEvent.y);
				float b = 1 - 2 * (quatRotationEvent.x * quatRotationEvent.x + quatRotationEvent.y * quatRotationEvent.y);

				float angRad = atan2(a, b);
				float angDeg = GGToDegree(angRad);

				if (angDeg < 0) angDeg += 360;
				if (angDeg > 360) angDeg -= 360;

				float sx = ObjectSizeX(iObj, 1)* 1.05f + g_fFlattenMargin;
				float sz = ObjectSizeZ(iObj, 1) * 1.05f + g_fFlattenMargin;
				int iFlattenID = t.entityelement[e].eleprof.iFlattenID;

				if (iFlattenID != -1)
				{
					//PE: This is draining the CPU, constantly updating while selected.
					static float Lastx = -1, Lasty = -1, Lastz = -1, Lastang = -1, LastSizeX = 0.0f, LastSizeZ = 0.0f;
					static int iLastFlattenID = -1;
					if (y != Lasty || x != Lastx || z != Lastz || angDeg != Lastang || LastSizeX != sx || LastSizeZ != sz || iFlattenID != iLastFlattenID)
					{
						Lastx = x;
						Lasty = y;
						Lastz = z;
						Lastang = angDeg;
						LastSizeX = sx;
						LastSizeZ = sz;
						iLastFlattenID = iFlattenID;
						GGTerrain_UpdateFlatArea(iFlattenID, x, z, angDeg, sx, sz, t.entityelement[e].y);
					}
				}
			}
		}
	}
}


void entity_autoFlattenWhenAdded(int e, int obj)
{
	int entid = t.entityelement[e].bankindex;
	if (entid > 0)
	{
		int iAutoFlattenMode = t.entityprofile[entid].autoflatten;
		if (iAutoFlattenMode != 0)
		{
			int iObj = t.entityelement[e].obj;
			if (iObj == 0 && obj > 0) iObj = obj;

			//LB: never create autoflatten entity instance is a smart object instance
			if (t.entityelement[e].iIsSmarkobjectDummyObj == 1)
				return;

			// ZJ: when a map is saved, the level gets reloaded and iIsSmarkobjectDummyObj gets wiped out.
			// g_smartObjectDummyEntities stores all entities with iIsSmarkobjectDummyObj set before save
			extern std::vector<int> g_smartObjectDummyEntities;
			for (auto& dummyID : g_smartObjectDummyEntities)
			{
				if (dummyID == e)
				{
					return;
				}
			}
			// Also need to ensure that any levels that were saved with the infinite hole bug are restored
			// It's a hack, but not sure how else old levels can be fixed
			if (t.entityelement[e].x == 0 && t.entityelement[e].y == -500000 && t.entityelement[e].z == 0)
			{
				return;
			}

			float x = t.entityelement[e].x + (GetObjectCollisionCenterX(iObj) * (ObjectScaleX(iObj) / 100.0f));
			float y = t.entityelement[e].y + (GetObjectCollisionCenterY(iObj) * (ObjectScaleY(iObj) / 100.0f)) - (ObjectSizeY(iObj) / 2, 1);
			float z = t.entityelement[e].z + (GetObjectCollisionCenterZ(iObj) * (ObjectScaleZ(iObj) / 100.0f));

			GGQUATERNION QuatAroundX, QuatAroundY, QuatAroundZ, quatRotationEvent;
			GGQuaternionRotationAxis(&QuatAroundX, &GGVECTOR3(1, 0, 0), GGToRadian(ObjectAngleX(iObj)));
			GGQuaternionRotationAxis(&QuatAroundY, &GGVECTOR3(0, 1, 0), GGToRadian(ObjectAngleY(iObj)));
			GGQuaternionRotationAxis(&QuatAroundZ, &GGVECTOR3(0, 0, 1), GGToRadian(ObjectAngleZ(iObj)));
			quatRotationEvent = QuatAroundX * QuatAroundY * QuatAroundZ;
			float a = 2 * (quatRotationEvent.x * quatRotationEvent.z + quatRotationEvent.w * quatRotationEvent.y);
			float b = 1 - 2 * (quatRotationEvent.x * quatRotationEvent.x + quatRotationEvent.y * quatRotationEvent.y);
			float angRad = atan2(a, b);
			float angDeg = GGToDegree(angRad);
			if (angDeg < 0) angDeg += 360;
			if (angDeg > 360) angDeg -= 360;

			float sx = ObjectSizeX(iObj, 1) * 1.05f + g_fFlattenMargin;
			float sz = ObjectSizeZ(iObj, 1) * 1.05f + g_fFlattenMargin;

			if ((!g_bEnableAutoFlattenSystem || !t.entityelement[e].eleprof.bAutoFlatten))
			{
				if (t.entityelement[e].eleprof.iFlattenID != -1)
				{
					GGTerrain_RemoveFlatArea(t.entityelement[e].eleprof.iFlattenID);
					t.entityelement[e].eleprof.iFlattenID = -1;
				}
			}
			else if (t.entityelement[e].eleprof.iFlattenID == -1)
			{
				if (g_bEnableAutoFlattenSystem == true)
				{
					if (iAutoFlattenMode == 1)
					{
						t.entityelement[e].eleprof.iFlattenID = GGTerrain_AddFlatRect(x, z, sx, sz, angDeg, t.entityelement[e].y);
					}
					else
					{
						float s = ObjectSize(t.entityelement[e].obj, 1) * 1.05f + g_fFlattenMargin;
						t.entityelement[e].eleprof.iFlattenID = GGTerrain_AddFlatCircle(x, z, s, t.entityelement[e].y);
					}
				}
			}
			else
			{
				GGTerrain_UpdateFlatArea(t.entityelement[e].eleprof.iFlattenID, x, z, angDeg, sx, sz, t.entityelement[e].y);
			}
		}
		else
		{
			if (t.entityelement[e].eleprof.iFlattenID != -1)
			{
				GGTerrain_RemoveFlatArea(t.entityelement[e].eleprof.iFlattenID);
				t.entityelement[e].eleprof.iFlattenID = -1;
			}
		}
	}
}

bool ObjectIsEntity(void* pTestObject)
{
	sObject* pObject = (sObject*)pTestObject;
	for (int te = 1; te <= g.entityelementlist; te++)
	{
		if (t.entityelement[te].obj > 0 )
		{
			if (pObject->dwObjectNumber == t.entityelement[te].obj)
				return true;
		}
	}
	return false;
}

//These functions need a pMesh overwrite. so we later can save into map data, for per object material changes.
//pMesh overwrite will not work ,so everything is now moved to t.entityelement[ele_id].eleprof.bCustomWickedMaterialActive

//PE: if Element ID set , use t.entityelement[g_iWickedElementId].eleprof.WEMaterial for everything.
//PE: "Materials" to limit map.ele size we only allow changes to mesh 1 , and per object settings (checkbox+reflectence).

void WickedSetElementId(int ele_id)
{
	//Only set if bCustomWickedMaterialActive has been activated by user.
	if (ele_id > 0 && t.entityelement[ele_id].eleprof.bCustomWickedMaterialActive)
	{
		g_iWickedElementId = ele_id;
	}
	else
	{
		g_iWickedElementId = 0;
	}
}

void WickedSetUseEditorGrideleprof(bool bUse)
{
	g_bUseEditorGrideleprof = bUse;
}

bool IsWickedMaterialActive(void* pvMesh)
{
	sMesh* pMesh = (sMesh*)pvMesh;
	if(g_iWickedEntityId < 0) return false;
	if( g_iWickedElementId > 0 && t.entityelement[g_iWickedElementId].eleprof.WEMaterial.MaterialActive )
		return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.MaterialActive;
	return t.entityprofile[g_iWickedEntityId].WEMaterial.MaterialActive;
}

void WickedSetEntityId(int ent_id)
{
	g_iWickedEntityId = ent_id;
}
int WickedGetEntityId(void)
{
	return g_iWickedEntityId;
}

float WickedGetTreeAlphaRef(void)
{
	float TreeAlphaRef = 1.0f;
	if (g_iWickedEntityId >= 0) { //PE: auto thumbs is now using 0.
		if (strcmp(Lower(t.entityprofile[g_iWickedEntityId].effect_s.Get()), "effectbank\\reloaded\\apbr_tree.fx") == 0)
		{
			TreeAlphaRef = 0.41f;
		}
		if (strcmp(Lower(t.entityprofile[g_iWickedEntityId].effect_s.Get()), "effectbank\\reloaded\\apbr_treea.fx") == 0)
		{
			TreeAlphaRef = 0.41f;
		}
	}
	return(TreeAlphaRef);
}

void WickedSetMeshNumber(int iMNumber)
{
	//PE: Got a crash when reaching 100 as index is only 0-99
	if (g_iWickedEntityId < 0 && iMNumber >= MAXMESHMATERIALS - 1)
		g_iWickedMeshNumber = 0;
	else
	{
		if(iMNumber >= MAXMESHMATERIALS - 1)
			g_iWickedMeshNumber = 0;
		else
			g_iWickedMeshNumber = iMNumber;
	}
}

cStr ReturnEmpty = "";

cStr WickedGetBaseColorName( void )
{
	if (g_iWickedEntityId < 0) return ReturnEmpty;
	if ( g_iWickedElementId > 0 && t.entityelement[g_iWickedElementId].eleprof.WEMaterial.MaterialActive)
	{
		if (t.entityelement[g_iWickedElementId].eleprof.WEMaterial.baseColorMapName[g_iWickedMeshNumber].Len() > 0) 
		{
			return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.baseColorMapName[g_iWickedMeshNumber];
		}
		return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.baseColorMapName[g_iWickedMeshNumber];
	}
	if (t.entityprofile[g_iWickedEntityId].WEMaterial.baseColorMapName[g_iWickedMeshNumber].Len() > 0) 
	{
		return t.entityprofile[g_iWickedEntityId].WEMaterial.baseColorMapName[g_iWickedMeshNumber];
	}
	if (g_iWickedMeshNumber >= 0 && g_iWickedMeshNumber < MAXMESHMATERIALS ) 
	{
		return t.entityprofile[g_iWickedEntityId].WEMaterial.baseColorMapName[g_iWickedMeshNumber];
	}
	return ReturnEmpty;
}

cStr WickedGetNormalName(void)
{
	if (g_iWickedEntityId < 0) return ReturnEmpty;
	if (g_iWickedElementId > 0 && t.entityelement[g_iWickedElementId].eleprof.WEMaterial.MaterialActive)
	{
		if (t.entityelement[g_iWickedElementId].eleprof.WEMaterial.baseColorMapName[g_iWickedMeshNumber].Len() > 0) 
		{
			return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.normalMapName[g_iWickedMeshNumber];
		}
		return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.normalMapName[g_iWickedMeshNumber];
	}
	if (t.entityprofile[g_iWickedEntityId].WEMaterial.normalMapName[g_iWickedMeshNumber].Len() > 0) 
	{
		return t.entityprofile[g_iWickedEntityId].WEMaterial.normalMapName[g_iWickedMeshNumber];
	}
	if (g_iWickedMeshNumber >= 0 && g_iWickedMeshNumber < MAXMESHMATERIALS) 
	{
		return t.entityprofile[g_iWickedEntityId].WEMaterial.normalMapName[g_iWickedMeshNumber];
	}
	return ReturnEmpty;
}

cStr WickedGetSurfaceName(void)
{
	if (g_iWickedEntityId < 0) return ReturnEmpty;
	if (g_iWickedElementId > 0 && t.entityelement[g_iWickedElementId].eleprof.WEMaterial.MaterialActive)
	{
		if (t.entityelement[g_iWickedElementId].eleprof.WEMaterial.baseColorMapName[g_iWickedMeshNumber].Len() > 0) 
		{
			return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.surfaceMapName[g_iWickedMeshNumber];
		}
		return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.surfaceMapName[g_iWickedMeshNumber];
	}
	if (t.entityprofile[g_iWickedEntityId].WEMaterial.surfaceMapName[g_iWickedMeshNumber].Len() > 0) 
	{
		return t.entityprofile[g_iWickedEntityId].WEMaterial.surfaceMapName[g_iWickedMeshNumber];
	}
	if (g_iWickedMeshNumber >= 0 && g_iWickedMeshNumber < MAXMESHMATERIALS) 
	{
		return t.entityprofile[g_iWickedEntityId].WEMaterial.surfaceMapName[g_iWickedMeshNumber];
	}
	return ReturnEmpty;
}

cStr WickedGetDisplacementName(void)
{
	if (g_iWickedEntityId < 0) return ReturnEmpty;
	if (g_iWickedElementId > 0 && t.entityelement[g_iWickedElementId].eleprof.WEMaterial.MaterialActive)
	{
		if (t.entityelement[g_iWickedElementId].eleprof.WEMaterial.baseColorMapName[g_iWickedMeshNumber].Len() > 0) 
		{
			return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.displacementMapName[g_iWickedMeshNumber];
		}
		return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.displacementMapName[g_iWickedMeshNumber];
	}
	if (t.entityprofile[g_iWickedEntityId].WEMaterial.displacementMapName[g_iWickedMeshNumber].Len() > 0) 
	{
		return t.entityprofile[g_iWickedEntityId].WEMaterial.displacementMapName[g_iWickedMeshNumber];
	}
	if (g_iWickedMeshNumber >= 0 && g_iWickedMeshNumber < MAXMESHMATERIALS) 
	{
		return t.entityprofile[g_iWickedEntityId].WEMaterial.displacementMapName[g_iWickedMeshNumber];
	}
	return ReturnEmpty;
}

cStr WickedGetEmissiveName(void)
{
	if (g_iWickedEntityId < 0) return ReturnEmpty;
	if (g_iWickedElementId > 0 && t.entityelement[g_iWickedElementId].eleprof.WEMaterial.MaterialActive)
	{
		if (t.entityelement[g_iWickedElementId].eleprof.WEMaterial.baseColorMapName[g_iWickedMeshNumber].Len() > 0) 
		{
			return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.emissiveMapName[g_iWickedMeshNumber];
		}
		return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.emissiveMapName[g_iWickedMeshNumber];
	}
	if (t.entityprofile[g_iWickedEntityId].WEMaterial.emissiveMapName[g_iWickedMeshNumber].Len() > 0) 
	{
		return t.entityprofile[g_iWickedEntityId].WEMaterial.emissiveMapName[g_iWickedMeshNumber];
	}
	if (g_iWickedMeshNumber >= 0 && g_iWickedMeshNumber < MAXMESHMATERIALS) 
	{
		return t.entityprofile[g_iWickedEntityId].WEMaterial.emissiveMapName[g_iWickedMeshNumber];
	}
	return ReturnEmpty;
}

cStr WickedGetOcclusionName(void)
{
	#ifdef DISABLEOCCLUSIONMAP
	if (g_iWickedEntityId < 0) return ReturnEmpty;
	if (t.entityprofile[g_iWickedEntityId].WEMaterial.baseColorMapName[g_iWickedMeshNumber].Len() > 0)
	{
		LPSTR pBaseTextureFilename = t.entityprofile[g_iWickedEntityId].WEMaterial.baseColorMapName[g_iWickedMeshNumber].Get();
		LPSTR pBaseColorFileExt = "_color.dds";
		if (strnicmp(pBaseTextureFilename + strlen(pBaseTextureFilename) - strlen(pBaseColorFileExt), pBaseColorFileExt, strlen(pBaseColorFileExt)-3) != NULL)
		{
			pBaseColorFileExt = "_basecolor.dds";
			if (strnicmp(pBaseTextureFilename + strlen(pBaseTextureFilename) - strlen(pBaseColorFileExt), pBaseColorFileExt, strlen(pBaseColorFileExt) - 3) != NULL)
			{
				// no matches found (ignoring file extension (dds,png))
				pBaseColorFileExt = "";
			}
		}
		if(strlen(pBaseColorFileExt)>0)
		{
			char pFullOcclusionFilename[MAX_PATH];
			strcpy(pFullOcclusionFilename, pBaseTextureFilename);
			pFullOcclusionFilename[strlen(pBaseTextureFilename) - strlen(pBaseColorFileExt)] = 0;
			char pFinalFilename[MAX_PATH];
			strcpy(pFinalFilename, pFullOcclusionFilename);
			strcat(pFinalFilename, "_height.dds");
			extern std::string g_pWickedTexturePath;
			cstr ensureCorrectFormatExt = cstr((LPSTR)g_pWickedTexturePath.c_str()) + pFinalFilename;
			if (FileExist((LPSTR)ensureCorrectFormatExt.Get()) == 0)
			{
				strcpy(pFinalFilename, pFullOcclusionFilename);
				strcat(pFinalFilename, "_height.png");
				ensureCorrectFormatExt = cstr((LPSTR)g_pWickedTexturePath.c_str()) + pFinalFilename;
			}
			cStr fullOcclusionFilename_s = ensureCorrectFormatExt;
			return fullOcclusionFilename_s;
		}
	}
	return ReturnEmpty;
	#else
	if (g_iWickedEntityId < 0) return ReturnEmpty;
	if (g_iWickedElementId > 0 && t.entityelement[g_iWickedElementId].eleprof.WEMaterial.MaterialActive)
	{
		if (t.entityelement[g_iWickedElementId].eleprof.WEMaterial.baseColorMapName[g_iWickedMeshNumber].Len() > 0) 
		{
			return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.occlusionMapName[g_iWickedMeshNumber];
		}
		return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.occlusionMapName[g_iWickedMeshNumber];
	}
	if (t.entityprofile[g_iWickedEntityId].WEMaterial.occlusionMapName[g_iWickedMeshNumber].Len() > 0) 
	{
		return t.entityprofile[g_iWickedEntityId].WEMaterial.occlusionMapName[g_iWickedMeshNumber];
	}
	if (g_iWickedMeshNumber >= 0 && g_iWickedMeshNumber < MAXMESHMATERIALS) 
	{
		return t.entityprofile[g_iWickedEntityId].WEMaterial.occlusionMapName[g_iWickedMeshNumber];
	}
	return ReturnEmpty;
	#endif
}

float WickedGetNormalStrength(void)
{
	if (g_iWickedEntityId < 0) return 1.0f;
	if (g_iWickedElementId > 0 && t.entityelement[g_iWickedElementId].eleprof.WEMaterial.MaterialActive)
	{
		if (t.entityelement[g_iWickedElementId].eleprof.WEMaterial.baseColorMapName[g_iWickedMeshNumber].Len() > 0) 
		{
			if(t.entityelement[g_iWickedElementId].eleprof.WEMaterial.fNormal[g_iWickedMeshNumber] >= 0.0f)
				return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.fNormal[g_iWickedMeshNumber];
			else
				return(1.0f);
		}
		return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.fNormal[g_iWickedMeshNumber];
	}
	if (t.entityprofile[g_iWickedEntityId].WEMaterial.fNormal[g_iWickedMeshNumber] >= 0.0) 
	{
		return t.entityprofile[g_iWickedEntityId].WEMaterial.fNormal[g_iWickedMeshNumber];
	}
	if (g_iWickedMeshNumber >= 0 && g_iWickedMeshNumber < MAXMESHMATERIALS && t.entityprofile[g_iWickedEntityId].WEMaterial.fNormal[g_iWickedMeshNumber] >= 0.0) 
	{
		return t.entityprofile[g_iWickedEntityId].WEMaterial.fNormal[g_iWickedMeshNumber];
	}
	return 1.0f;
}

float WickedGetRoughnessStrength(void)
{
	if (g_iWickedEntityId < 0) return 0.2f;
	if (g_iWickedElementId > 0 && t.entityelement[g_iWickedElementId].eleprof.WEMaterial.MaterialActive)
	{
		if (t.entityelement[g_iWickedElementId].eleprof.WEMaterial.baseColorMapName[g_iWickedMeshNumber].Len() > 0) 
		{
			if (t.entityelement[g_iWickedElementId].eleprof.WEMaterial.fRoughness[g_iWickedMeshNumber] >= 0.0f)
				return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.fRoughness[g_iWickedMeshNumber];
			else
				return(0.2f);
		}
		return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.fRoughness[g_iWickedMeshNumber];
	}
	if (t.entityprofile[g_iWickedEntityId].WEMaterial.fRoughness[g_iWickedMeshNumber] >= 0.0) 
	{
		return t.entityprofile[g_iWickedEntityId].WEMaterial.fRoughness[g_iWickedMeshNumber];
	}
	if (g_iWickedMeshNumber >= 0 && g_iWickedMeshNumber < MAXMESHMATERIALS && t.entityprofile[g_iWickedEntityId].WEMaterial.fRoughness[g_iWickedMeshNumber] >= 0.0) 
	{
		return t.entityprofile[g_iWickedEntityId].WEMaterial.fRoughness[g_iWickedMeshNumber];
	}
	return 0.2f;
}
float WickedGetMetallnessStrength(void)
{
	if (g_iWickedEntityId < 0) return 0.0f;
	if (g_iWickedElementId > 0 && t.entityelement[g_iWickedElementId].eleprof.WEMaterial.MaterialActive)
	{
		if (t.entityelement[g_iWickedElementId].eleprof.WEMaterial.baseColorMapName[g_iWickedMeshNumber].Len() > 0) 
		{
			if (t.entityelement[g_iWickedElementId].eleprof.WEMaterial.fMetallness[g_iWickedMeshNumber] >= 0.0f)
				return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.fMetallness[g_iWickedMeshNumber];
			else
				return(0.0f);
		}
		return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.fMetallness[g_iWickedMeshNumber];
	}
	if (t.entityprofile[g_iWickedEntityId].WEMaterial.fMetallness[g_iWickedMeshNumber] >= 0.0) 
	{
		return t.entityprofile[g_iWickedEntityId].WEMaterial.fMetallness[g_iWickedMeshNumber];
	}
	if (g_iWickedMeshNumber >= 0 && g_iWickedMeshNumber < MAXMESHMATERIALS && t.entityprofile[g_iWickedEntityId].WEMaterial.fMetallness[g_iWickedMeshNumber] >= 0.0) 
	{
		return t.entityprofile[g_iWickedEntityId].WEMaterial.fMetallness[g_iWickedMeshNumber];
	}
	return 0.0f;
}

float WickedGetEmissiveStrength(void)
{
	if (g_iWickedEntityId < 0) return 0.0f;
	if (g_iWickedElementId > 0 && t.entityelement[g_iWickedElementId].eleprof.WEMaterial.MaterialActive)
	{
		if (t.entityelement[g_iWickedElementId].eleprof.WEMaterial.baseColorMapName[g_iWickedMeshNumber].Len() > 0) 
		{
			if (t.entityelement[g_iWickedElementId].eleprof.WEMaterial.fEmissive[g_iWickedMeshNumber] >= 0.0f)
				return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.fEmissive[g_iWickedMeshNumber];
			else
				return(0.0f);
		}
		return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.fEmissive[g_iWickedMeshNumber];
	}
	if (t.entityprofile[g_iWickedEntityId].WEMaterial.fEmissive[g_iWickedMeshNumber] >= 0.0) 
	{
		return t.entityprofile[g_iWickedEntityId].WEMaterial.fEmissive[g_iWickedMeshNumber];
	}
	if (g_iWickedMeshNumber >= 0 && g_iWickedMeshNumber < MAXMESHMATERIALS && t.entityprofile[g_iWickedEntityId].WEMaterial.fEmissive[g_iWickedMeshNumber] >= 0.0) 
	{
		return t.entityprofile[g_iWickedEntityId].WEMaterial.fEmissive[g_iWickedMeshNumber];
	}
	return 0.0f;
}

float WickedGetAlphaRef(void)
{
	if (g_iWickedEntityId < 0) return 1.0f;
	if (g_iWickedElementId > 0 && t.entityelement[g_iWickedElementId].eleprof.WEMaterial.MaterialActive)
	{
		if (t.entityelement[g_iWickedElementId].eleprof.WEMaterial.baseColorMapName[g_iWickedMeshNumber].Len() > 0) 
		{
			if (t.entityelement[g_iWickedElementId].eleprof.WEMaterial.fAlphaRef[g_iWickedMeshNumber] < 0)
				return(1.0f);
			return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.fAlphaRef[g_iWickedMeshNumber];
		}
		if (t.entityelement[g_iWickedElementId].eleprof.WEMaterial.fAlphaRef[g_iWickedMeshNumber] < 0)
			return(1.0f);
		return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.fAlphaRef[g_iWickedMeshNumber];
	}
	if (t.entityprofile[g_iWickedEntityId].WEMaterial.fAlphaRef[g_iWickedMeshNumber] >= 0.0) 
	{
		return t.entityprofile[g_iWickedEntityId].WEMaterial.fAlphaRef[g_iWickedMeshNumber];
	}
	if (g_iWickedMeshNumber >= 0 && g_iWickedMeshNumber < MAXMESHMATERIALS && t.entityprofile[g_iWickedEntityId].WEMaterial.fAlphaRef[g_iWickedMeshNumber] >= 0.0) 
	{
		return t.entityprofile[g_iWickedEntityId].WEMaterial.fAlphaRef[g_iWickedMeshNumber];
	}
	return 1.0f;
}

bool WickedDoubleSided(void)
{
	if (g_iWickedEntityId < 0) return false;
	if (g_iWickedElementId > 0 && t.entityelement[g_iWickedElementId].eleprof.WEMaterial.MaterialActive)
	{
		return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.bDoubleSided[g_iWickedMeshNumber];
	}
	return t.entityprofile[g_iWickedEntityId].WEMaterial.bDoubleSided[g_iWickedMeshNumber];
}

int WickedCustomShaderID(void)
{
	if (g_iWickedEntityId < 0) return -1;
	if (g_iWickedElementId > 0 && t.entityelement[g_iWickedElementId].eleprof.WEMaterial.MaterialActive)
	{
		return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.customShaderID;
	}
	return t.entityprofile[g_iWickedEntityId].WEMaterial.customShaderID;
}
float WickedCustomShaderParam1(void)
{
	if (g_iWickedEntityId < 0) return 0;
	if (g_iWickedElementId > 0 && t.entityelement[g_iWickedElementId].eleprof.WEMaterial.MaterialActive)
	{
		return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.customShaderParam1;
	}
	return t.entityprofile[g_iWickedEntityId].WEMaterial.customShaderParam1;
}
float WickedCustomShaderParam2(void)
{
	if (g_iWickedEntityId < 0) return 0;
	if (g_iWickedElementId > 0 && t.entityelement[g_iWickedElementId].eleprof.WEMaterial.MaterialActive)
	{
		return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.customShaderParam2;
	}
	return t.entityprofile[g_iWickedEntityId].WEMaterial.customShaderParam2;
}
float WickedCustomShaderParam3(void)
{
	if (g_iWickedEntityId < 0) return 0;
	if (g_iWickedElementId > 0 && t.entityelement[g_iWickedElementId].eleprof.WEMaterial.MaterialActive)
	{
		return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.customShaderParam3;
	}
	return t.entityprofile[g_iWickedEntityId].WEMaterial.customShaderParam3;
}
float WickedCustomShaderParam4(void)
{
	if (g_iWickedEntityId < 0) return 0;
	if (g_iWickedElementId > 0 && t.entityelement[g_iWickedElementId].eleprof.WEMaterial.MaterialActive)
	{
		return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.customShaderParam4;
	}
	return t.entityprofile[g_iWickedEntityId].WEMaterial.customShaderParam4;
}
float WickedCustomShaderParam5(void)
{
	if (g_iWickedEntityId < 0) return 0;
	if (g_iWickedElementId > 0 && t.entityelement[g_iWickedElementId].eleprof.WEMaterial.MaterialActive)
	{
		return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.customShaderParam5;
	}
	return t.entityprofile[g_iWickedEntityId].WEMaterial.customShaderParam5;
}
float WickedCustomShaderParam6(void)
{
	if (g_iWickedEntityId < 0) return 0;
	if (g_iWickedElementId > 0 && t.entityelement[g_iWickedElementId].eleprof.WEMaterial.MaterialActive)
	{
		return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.customShaderParam6;
	}
	return t.entityprofile[g_iWickedEntityId].WEMaterial.customShaderParam6;
}
float WickedCustomShaderParam7(void)
{
	if (g_iWickedEntityId < 0) return 0;
	if (g_iWickedElementId > 0 && t.entityelement[g_iWickedElementId].eleprof.WEMaterial.MaterialActive)
	{
		return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.customShaderParam7;
	}
	return t.entityprofile[g_iWickedEntityId].WEMaterial.customShaderParam7;
}

float WickedRenderOrderBias(void)
{
	if (g_iWickedEntityId < 0) return false;
	if (g_iWickedElementId > 0 && t.entityelement[g_iWickedElementId].eleprof.WEMaterial.MaterialActive)
	{
		return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.fRenderOrderBias[g_iWickedMeshNumber];
	}
	return t.entityprofile[g_iWickedEntityId].WEMaterial.fRenderOrderBias[g_iWickedMeshNumber];
}

bool WickedGetTransparent(void)
{
	bool bIsTransparent = false;
	if (g_iWickedEntityId < 0) return false;
	if (g_iWickedElementId > 0 && t.entityelement[g_iWickedElementId].eleprof.WEMaterial.MaterialActive)
		bIsTransparent = t.entityelement[g_iWickedElementId].eleprof.WEMaterial.bTransparency[g_iWickedMeshNumber];
	else
		bIsTransparent = t.entityprofile[g_iWickedEntityId].WEMaterial.bTransparency[g_iWickedMeshNumber];

	return(bIsTransparent);
}

bool WickedGetCastShadows(void)
{
	if (g_iWickedEntityId < 0) return false;
	if (g_iWickedElementId > 0 && t.entityelement[g_iWickedElementId].eleprof.WEMaterial.MaterialActive)
	{
		return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.bCastShadows[g_iWickedMeshNumber];
	}
	return t.entityprofile[g_iWickedEntityId].WEMaterial.bCastShadows[g_iWickedMeshNumber];
}

bool WickedPlanerReflection(void)
{
	if (g_iWickedEntityId < 0) return false;
	if (g_iWickedElementId > 0 && t.entityelement[g_iWickedElementId].eleprof.WEMaterial.MaterialActive)
	{
		return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.bPlanerReflection[g_iWickedMeshNumber];
	}
	return t.entityprofile[g_iWickedEntityId].WEMaterial.bPlanerReflection[g_iWickedMeshNumber];
}

float WickedGetReflectance(void)
{
	if (g_iWickedEntityId < 0)
		return 0.04f;// 0.002f;
	if (g_iWickedElementId > 0 && t.entityelement[g_iWickedElementId].eleprof.WEMaterial.MaterialActive)
	{
		return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.fReflectance[g_iWickedMeshNumber];
	}
	return t.entityprofile[g_iWickedEntityId].WEMaterial.fReflectance[g_iWickedMeshNumber];
}

DWORD WickedGetEmmisiveColor(void)
{
	if (g_iWickedEntityId < 0) return -1;
	if (g_iWickedElementId > 0 && t.entityelement[g_iWickedElementId].eleprof.WEMaterial.MaterialActive)
	{
		return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.dwEmmisiveColor[g_iWickedMeshNumber];
	}
	return t.entityprofile[g_iWickedEntityId].WEMaterial.dwEmmisiveColor[g_iWickedMeshNumber];
}

DWORD WickedGetBaseColor(void)
{
	if (g_iWickedEntityId < 0) return -1;
	if (g_iWickedElementId > 0 && t.entityelement[g_iWickedElementId].eleprof.WEMaterial.MaterialActive)
	{
		return t.entityelement[g_iWickedElementId].eleprof.WEMaterial.dwBaseColor[g_iWickedMeshNumber];
	}
	return t.entityprofile[g_iWickedEntityId].WEMaterial.dwBaseColor[g_iWickedMeshNumber];
}

void Wicked_Highlight_ClearAllObjects(void)
{
	for (int e = 1; e <= g.entityelementlist; e++)
	{
		int obj = t.entityelement[e].obj;
		if (obj > 0)
		{
			sObject* pObject = GetObjectData(obj);
			if (pObject)
			{
				WickedCall_SetObjectHighlightBlue(pObject, false);
			}
		}
	}
}

void Wicked_Highlight_AllLogicObjects(void)
{
	for (int e = 1; e <= g.entityelementlist; e++)
	{
		// only highlight the logic objects in blue
		if (t.entityelement[e].staticflag == 0)
		{
			int obj = t.entityelement[e].obj;
			if (obj > 0)
			{
				sObject* pObject = GetObjectData(obj);
				if (pObject)
				{
					WickedCall_SetObjectHighlightBlue(pObject, true);
					g_ObjectHighlightList.push_back(obj);
				}
			}
		}
	}
}

void Wicked_Highlight_Rubberband(void)
{
	if (t.grideditselect == 5 && g.entityrubberbandlist.size() > 0)
	{
		for (int i = 0; i < (int)g.entityrubberbandlist.size(); i++)
		{
			int e = g.entityrubberbandlist[i].e;
			if (e >= t.entityelement.size())
			{
				g.entityrubberbandlist.clear();
				return;
			}
			int tobj = t.entityelement[e].obj;
			if (tobj > 0)
			{
				sObject* pObject = GetObjectData(tobj);
				if (pObject) {
					void WickedCall_DrawObjctBox(sObject* pObject, XMFLOAT4 color, bool bThickLine = false, bool ForceBox = false);
					WickedCall_DrawObjctBox(pObject, XMFLOAT4(0.75f, 0.75f, 0.75f, 0.75f),false,false);
				}
			}
		}
	}
}

std::vector<sRubberBandType> entityselectionlist;
void Wicked_Highlight_Selection(void)
{
	if (t.grideditselect == 5 && entityselectionlist.size() > 0)
	{
		for (int i = 0; i < (int)entityselectionlist.size(); i++)
		{
			int e = entityselectionlist[i].e;
			int tobj = t.entityelement[e].obj;
			if (tobj > 0)
			{
				sObject* pObject = GetObjectData(tobj);
				if (pObject) {
					void WickedCall_DrawObjctBox(sObject* pObject, XMFLOAT4 color, bool bThickLine = false, bool ForceBox = false);
					WickedCall_DrawObjctBox(pObject, XMFLOAT4(1.0f, 0.75f, 0.0f, 0.85f), true,true);
				}
			}
		}
	}
}

extern std::vector<sRubberBandType> vEntityLockedList;
void Wicked_Highlight_LockedList(void)
{
	if (t.grideditselect == 5 && vEntityLockedList.size() > 0)
	{
		for (int i = 0; i < (int)vEntityLockedList.size(); i++)
		{
			int e = vEntityLockedList[i].e;
			if (e < 0 || e >= t.entityelement.size()) continue;

			#ifdef ALLOWSELECTINGLOCKEDOBJECTS
			//PE: Only diplay red on selected items.
			bool bDisplayRed = false;
			if (e == t.widget.pickedEntityIndex)
				bDisplayRed = true;
			if (!bDisplayRed)
			{
				//PE: Also need rubberband items. (if within a selected group).
				if (g.entityrubberbandlist.size() > 0)
				{
					for (int ir = 0; ir < (int)g.entityrubberbandlist.size(); ir++)
					{
						if (e == g.entityrubberbandlist[ir].e)
						{
							bDisplayRed = true;
							break;
						}
					}
				}
			}
				
			if(bDisplayRed)
			{
			#endif

				int tobj = t.entityelement[e].obj;
				if (tobj > 0)
				{
					sObject* pObject = GetObjectData(tobj);
					if (pObject) {
						void WickedCall_DrawObjctBox(sObject* pObject, XMFLOAT4 color, bool bThickLine = false, bool ForceBox = false);
						WickedCall_DrawObjctBox(pObject, XMFLOAT4(1.0f, 0.0f, 0.0f, 0.85f), false, false);
					}
				}
			#ifdef ALLOWSELECTINGLOCKEDOBJECTS
			}
			#endif
		}
	}
}

void Wicked_Ignore_Frame_Mesh(int obj)
{
	if (obj <= 0 || obj > MAXIMUMVALUE) return;
	sObject* pObject = g_ObjectList[obj];
	if (!pObject) return;

	for (int i = 0; i < pObject->iFrameCount; i++)
	{
		if (pObject->ppFrameList[i])
		{
			pObject->ppFrameList[i]->bIgnoreMesh = false;
		}
	}

	extern bool bDisableLODLoad;
	if (bDisableLODLoad) return;

	int bestlod = -1;
	bool bHasLOD = false;

	for (int i = 0; i < pObject->iFrameCount; i++)
	{
		sFrame* pFrame = pObject->ppFrameList[i];
		if (pFrame)
		{
			LPSTR pName = pFrame->szName;
			if (pName && strlen(pName) > 0)
			{
				char* r5 = nullptr;
				cstr lname = Lower(pName);
				if (strlen(lname.Get()) >= 5)
				{
					r5 = lname.Get() + strlen(lname.Get()) - 5;
					if (r5)
					{
						if ((stricmp(r5, "lod_1") == 0 || stricmp(r5, "_lod1") == 0))
							bHasLOD = true;
						if ((stricmp(r5, "lod_2") == 0 || stricmp(r5, "_lod2") == 0))
							bHasLOD = true;

						if ((stricmp(r5, "lod_1") == 0 || stricmp(r5, "_lod1") == 0) && (bestlod == -1 || bestlod > 1))
						{
							bestlod = 1;
						}
						else
						{
							if ((stricmp(r5, "lod_2") == 0 || stricmp(r5, "_lod2") == 0) && (bestlod == -1))
							{
								bestlod = 2;
							}
							else
							{
								bestlod = 0;
							}
						}

					}
				}

				if (stricmp(lname.Get(), "collision_mesh") == 0)
				{
					pFrame->bIgnoreMesh = true;
				}
				//PE: UCX unreal collision mesh must include LOD
				if (strnicmp(lname.Get(), "UCX_", 4) == 0 && r5 && stricmp(r5, "_lod0") == 0)
				{
					pFrame->bIgnoreMesh = true;
				}
			}
		}
	}

	if (bHasLOD)
	{
		for (int i = 0; i < pObject->iFrameCount; i++)
		{
			sFrame* pFrame = pObject->ppFrameList[i];
			if (pFrame)
			{
				LPSTR pName = pFrame->szName;
				if (pName && strlen(pName) > 0)
				{
					char* r5 = nullptr;
					cstr lname = Lower(pName);
					if (strlen(lname.Get()) >= 5)
					{
						r5 = lname.Get() + strlen(lname.Get()) - 5;
						if (bestlod == 0 && (stricmp(r5, "lod_1") == 0 || stricmp(r5, "lod_2") == 0 || stricmp(r5, "lod_3") == 0))
						{
							pFrame->bIgnoreMesh = true;
						}
						if (bestlod == 0 && (stricmp(r5, "_lod1") == 0 || stricmp(r5, "_lod2") == 0 || stricmp(r5, "_lod3") == 0))
						{
							pFrame->bIgnoreMesh = true;
						}
						if (bestlod == 1 && (stricmp(r5, "lod_2") == 0 || stricmp(r5, "_lod2") == 0))
						{
							pFrame->bIgnoreMesh = true;
						}
						if (bestlod == 2 && (stricmp(r5, "lod_3") == 0 || stricmp(r5, "_lod3") == 0))
						{
							pFrame->bIgnoreMesh = true;
						}
					}
				}
			}
		}
	}
}

void Wicked_Hide_Lower_Lod_Meshes(int obj)
{
	PerformCheckListForLimbs(obj);
	int bestlod = -1;
	bool bHasLOD = false;
	for (t.c = ChecklistQuantity(); t.c >= 1; t.c += -1)
	{
		t.tname_s = Lower(ChecklistString(t.c));
		LPSTR pRightFive = "";
		if (strlen(t.tname_s.Get()) >= 5) pRightFive = t.tname_s.Get() + strlen(t.tname_s.Get()) - 5;

		if ((stricmp(pRightFive, "lod_1") == 0 || stricmp(pRightFive, "_lod1") == 0) )
			bHasLOD = true;
		if ((stricmp(pRightFive, "lod_2") == 0 || stricmp(pRightFive, "_lod2") == 0))
			bHasLOD = true;

		if ((stricmp(pRightFive, "lod_1") == 0 || stricmp(pRightFive, "_lod1") == 0) && (bestlod == -1 || bestlod > 1))
		{
			bestlod = 1;
		}
		else
		{
			if ((stricmp(pRightFive, "lod_2") == 0 || stricmp(pRightFive, "_lod2") == 0) && (bestlod == -1))
			{
				bestlod = 2;
			}
			else
			{
				// base (highest) LOD does not need the lod0 or _lod postfix
				bestlod = 0;
			}
		}

		// also hide ANY collision_mesh frame as this should never be visible
		if (stricmp(t.tname_s.Get(), "collision_mesh") == 0)
		{
			HideLimb(obj, t.c - 1);
		}
	}
	if (bHasLOD)
	{
		for (t.c = ChecklistQuantity(); t.c >= 1; t.c += -1)
		{
			t.tname_s = Lower(ChecklistString(t.c));
			LPSTR pRightFive = "";
			if (strlen(t.tname_s.Get()) >= 5) pRightFive = t.tname_s.Get() + strlen(t.tname_s.Get()) - 5;
			if (bestlod == 0 && (stricmp(pRightFive, "lod_1") == 0 || stricmp(pRightFive, "lod_2") == 0 || stricmp(pRightFive, "lod_3") == 0)) 
			{
				HideLimb(obj, t.c - 1);
			}
			if (bestlod == 0 && (stricmp(pRightFive, "_lod1") == 0 || stricmp(pRightFive, "_lod2") == 0 || stricmp(pRightFive, "_lod3") == 0)) 
			{
				HideLimb(obj, t.c - 1);
			}
			if (bestlod == 1 && (stricmp(pRightFive, "lod_2") == 0 || stricmp(pRightFive, "_lod2") == 0)) 
			{
				HideLimb(obj, t.c - 1);
			}
			if (bestlod == 2 && (stricmp(pRightFive, "lod_3") == 0 || stricmp(pRightFive, "_lod3") == 0)) 
			{
				HideLimb(obj, t.c - 1);
			}
		}
	}
}


int GetLodLevels(int obj)
{
	PerformCheckListForLimbs(obj);
	int iLodLevels = 0;
	int bestlod = -1;
	bool bHasLOD = false;
	for (t.c = ChecklistQuantity(); t.c >= 1; t.c += -1)
	{
		t.tname_s = Lower(ChecklistString(t.c));
		LPSTR pRightFive = "";
		if (strlen(t.tname_s.Get()) >= 5) pRightFive = t.tname_s.Get() + strlen(t.tname_s.Get()) - 5;
		if ((stricmp(pRightFive, "lod_1") == 0 || stricmp(pRightFive, "_lod1") == 0))
			iLodLevels++;
		else if ((stricmp(pRightFive, "lod_2") == 0 || stricmp(pRightFive, "_lod2") == 0))
			iLodLevels++;
		else if ((stricmp(pRightFive, "lod_3") == 0 || stricmp(pRightFive, "_lod3") == 0))
			iLodLevels++;

	}
	return(iLodLevels);
}

void entity_updatequatfromeuler (int e)
{
	// update entity quat as the preferred source rotation
	GGQUATERNION QuatAroundX, QuatAroundY, QuatAroundZ;
	GGQuaternionRotationAxis(&QuatAroundX, &GGVECTOR3(1, 0, 0), GGToRadian(t.entityelement[e].rx));
	GGQuaternionRotationAxis(&QuatAroundY, &GGVECTOR3(0, 1, 0), GGToRadian(t.entityelement[e].ry));
	GGQuaternionRotationAxis(&QuatAroundZ, &GGVECTOR3(0, 0, 1), GGToRadian(t.entityelement[e].rz));
	GGQUATERNION quatNewOrientation = QuatAroundX * QuatAroundY * QuatAroundZ;
	t.entityelement[e].quatmode = 1;
	t.entityelement[e].quatx = quatNewOrientation.x;
	t.entityelement[e].quaty = quatNewOrientation.y;
	t.entityelement[e].quatz = quatNewOrientation.z;
	t.entityelement[e].quatw = quatNewOrientation.w;
}

void entity_updatequat (int e, float quatx, float quaty, float quatz, float quatw)
{
	t.entityelement[e].quatmode = 1;
	t.entityelement[e].quatx = quatx;
	t.entityelement[e].quaty = quaty;
	t.entityelement[e].quatz = quatz;
	t.entityelement[e].quatw = quatw;
}

void entity_calculateeuleryfromquat (int e)
{
	if (t.entityelement[e].quatmode != 0 ) // seems some corruption can given this value a non-zero but still have quat rot data
	{
		// only do if have a good quat
		GGQUATERNION Quat = GGQUATERNION(t.entityelement[e].quatx, t.entityelement[e].quaty, t.entityelement[e].quatz, t.entityelement[e].quatw);
		GGMATRIX matQuatRot;
		GGMatrixRotationQuaternion(&matQuatRot, &Quat);
		GGVECTOR3 positionalOffset = GGVECTOR3(0, 0, 1);
		GGVec3TransformCoord(&positionalOffset, &positionalOffset, &matQuatRot);
		float fRealWorldYAngle = Atan2(positionalOffset.x, positionalOffset.z);
		t.entityelement[e].rx = 0;
		t.entityelement[e].ry = fRealWorldYAngle;
		t.entityelement[e].rz = 0;

		// ensure generated euler is exactly matching quat (and sets quatmode to 1 in case of older level corruption data)
		entity_updatequatfromeuler(e);
	}
	else
	{
		// otherwise we already have the euler values
	}
}

#ifndef GGMAXEPIC
#include "M-Workshop.h"
extern std::vector<sWorkshopItem> g_workshopItemsList;
extern std::vector<sWorkshopSteamUserName> g_workshopSteamUserNames;
extern std::vector<PublishedFileId_t> g_workshopTrustedItems;
bool workshop_verifyandorreplacescript(int e, int entid)
{
	return false;
}
#else
bool workshop_verifyandorreplacescript(int e, int entid)
{
	// does nothing in EPIC mode
	return false;
}
#endif
