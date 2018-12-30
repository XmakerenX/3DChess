#include "FbxLoader.h"
#include <iostream>
#include <exception>

//-----------------------------------------------------------------------------
// Name : LoadMesh
//-----------------------------------------------------------------------------
bool FbxLoader::LoadMesh(std::string meshPath, std::vector<SubMesh>& subMeshes)
{
    if (m_pFbxSdkManager == nullptr)
    {
        m_pFbxSdkManager = FbxManager::Create();

        FbxIOSettings* pIOsettings = FbxIOSettings::Create(m_pFbxSdkManager, IOSROOT);
        m_pFbxSdkManager->SetIOSettings(pIOsettings);
    }

    FbxImporter* pImporter = FbxImporter::Create(m_pFbxSdkManager, "");
    FbxScene* pFbxScene = FbxScene::Create(m_pFbxSdkManager,"");

    bool bSuccess = pImporter->Initialize(meshPath.c_str(), -1, m_pFbxSdkManager->GetIOSettings());
    if (!bSuccess)
    {
        std::cout << "Fbxloader: Failed to Initialzie Importer\n";
        return false;
    }

    bSuccess = pImporter->Import(pFbxScene);
    if (!bSuccess)
    {
        std::cout << "Fbxloader: Importer failed to Import scene\n";
        return false;
    }

    pImporter->Destroy();

    std::vector<Vertex> vertices;
    std::vector<VertexIndex> indices;

    std::vector<GLuint> meshMaterials;
    //std::vector<SubMesh> subMeshes;

    FbxNode* pFbxRootNode = pFbxScene->GetRootNode();
    if (pFbxRootNode)
    {
        for (int i = 0; i < pFbxRootNode->GetChildCount(); i++)
        {
            FbxNode* pFbxChildNode = pFbxRootNode->GetChild(i);

            if (pFbxChildNode->GetNodeAttribute() == nullptr)
                continue;

            FbxNodeAttribute::EType AttributeType = pFbxChildNode->GetNodeAttribute()->GetAttributeType();

            if (AttributeType != FbxNodeAttribute::eMesh)
                continue;

            FbxMesh* pMesh = (FbxMesh*)pFbxChildNode->GetNodeAttribute();

            FbxVector4* pFBXVertices = pMesh->GetControlPoints();
            GLuint vertexCount = 0;

            for (int j = 0; j < pMesh->GetPolygonCount(); j++)
            {
                int iNumVertices = pMesh->GetPolygonSize(j);
                assert(iNumVertices == 3);

                for (int k = 0; k < iNumVertices; k++)
                {
                    int iControlPointIndex = pMesh->GetPolygonVertex(j, k);

                    Vertex vertex;

                    vertex.Position.x = static_cast<float>(pFBXVertices[iControlPointIndex].mData[0]);
                    vertex.Position.y = static_cast<float>(pFBXVertices[iControlPointIndex].mData[1]);
                    vertex.Position.z = static_cast<float>(pFBXVertices[iControlPointIndex].mData[2]);

                    vertex.Normal = readFBXNormal(pMesh, iControlPointIndex, vertexCount);
                    vertex.TexCoords = readFBXUV(pMesh, iControlPointIndex, j, k);

                    vertices.push_back(vertex);
                    indices.push_back(vertexCount);

                    vertexCount++;
                }

            }

            subMeshes.emplace_back(vertices, indices);
        }

        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
// Name : readFBXNormal
//-----------------------------------------------------------------------------
glm::vec3 FbxLoader::readFBXNormal(FbxMesh* mesh, int controlPointIndex, int vertexCounter)
{
    glm::vec3 retNormal = glm::vec3(0.0f, 0.0f, 0.0f);
    if (mesh->GetElementNormalCount() < 1)
    {
        std::cout << "Invalid Normal Number\n";
        //TODO: find why std::expection wont fucking work
        //throw std::exception("Invalid Normal Number");
        return glm::vec3(0.0f, 0.0f, 0.0f);
    }

    FbxGeometryElementNormal* vertexNormal = mesh->GetElementNormal(0);
    switch (vertexNormal->GetMappingMode())
    {
    case FbxGeometryElement::eByControlPoint:
    {
        switch(vertexNormal->GetReferenceMode())
        {
        case FbxGeometryElement::eDirect:
        {
            retNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(controlPointIndex).mData[0]);
            retNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(controlPointIndex).mData[1]);
            retNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(controlPointIndex).mData[2]);
        }break;

        case FbxGeometryElement::eIndexToDirect:
        {
            int index = vertexNormal->GetIndexArray().GetAt(controlPointIndex);
            retNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
            retNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
            retNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
        }break;

        default:
            //TODO: figute out this expection throwing thing
            //throw std::exception("Invalid Reference");
            throw std::exception();

            }
        }break;

        case FbxGeometryElement::eByPolygonVertex:
        {
            switch(vertexNormal->GetReferenceMode())
            {
            case FbxGeometryElement::eDirect:
            {
                retNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(vertexCounter).mData[0]);
                retNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(vertexCounter).mData[1]);
                retNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(vertexCounter).mData[2]);
            }break;

            case FbxGeometryElement::eIndexToDirect:
            {
                int index = vertexNormal->GetIndexArray().GetAt(vertexCounter);
                retNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
                retNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
                retNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
            }break;

            default:
                //throw std::exception("Invalid Reference");
                throw std::exception();

            }
        }break;

        }
        return retNormal;
}

//-----------------------------------------------------------------------------
// Name : readFBXUV
//-----------------------------------------------------------------------------
glm::vec2 FbxLoader::readFBXUV(FbxMesh* mesh, int controlPointIndex, int j, int k)
{
    FbxVector2 uv;
    glm::vec2 retUV = glm::vec2(0.0f, 0.0f);

    FbxGeometryElementUV* pUV = mesh->GetElementUV(0);
    if (pUV == nullptr)
    {
        return glm::vec2(-1.0f, -1.0f);
    }
    else
    {
        switch(pUV->GetMappingMode())
        {
        case FbxGeometryElement::eByControlPoint:
        {
            switch(pUV->GetReferenceMode())
            {
            case FbxGeometryElement::eDirect:
            {
                uv = pUV->GetDirectArray().GetAt(controlPointIndex);
            }break;

            case FbxGeometryElement::eIndexToDirect:
            {
                int id = pUV->GetIndexArray().GetAt(controlPointIndex);
                uv = pUV->GetDirectArray().GetAt(id);
            }break;

            default:
                break;
            }
        }break;

        case FbxGeometryElement::eByPolygonVertex:
        {
            int textureUVIndex = mesh->GetTextureUVIndex(j, k);
            switch(pUV->GetReferenceMode())
            {
            case FbxGeometryElement::eDirect:
            case FbxGeometryElement::eIndexToDirect:
            {
                uv = pUV->GetDirectArray().GetAt(textureUVIndex);
            }break;

            default:
                break;
            }
        }break;

        default:
            break;

        }

        retUV.x = static_cast<float>(uv.mData[0]);
        retUV.y = static_cast<float>(uv.mData[1]);

    }

    return retUV;
}
