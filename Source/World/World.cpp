#include "World.h"

#include <iostream>
#include <unordered_map>
#include <SFML/Graphics.hpp>

#include "../Renderer/RMaster.h"

#include "../Player/Player.h"
#include "../Camera.h"
#include "../Maths/General_Maths.h"

World::World(const Camera& camera, int32_t worldSize)
:   m_p_camera      (&camera)
,   m_worldSize     (worldSize)
{
    for (int32_t x = 0 ; x < m_worldSize; x++)
    {
        for (int32_t z = 0; z < m_worldSize; z++)
        {
            m_chunks.addChunk({x, z}, *this);
        }
    }
}

void World::checkPlayerBounds(Player& player)
{
    if (player.position.x < 0.2f)
    {
        player.position.x = 0.2f;
    }
    if (player.position.z < 0.2f)
    {
        player.position.z = 0.2f;
    }

    if (player.position.x + 0.2f > m_worldSize * CHUNK_SIZE - 0.2f )
    {
        player.position.x = m_worldSize * CHUNK_SIZE - 0.3f;
    }
    if (player.position.z + 0.2f > m_worldSize * CHUNK_SIZE - 0.2f )
    {
        player.position.z = m_worldSize * CHUNK_SIZE - 0.3f;
    }
}

void World::setBlock(const Vector3& position, CBlock block)
{
    m_newBlocks.emplace_back(block, position);
}

CBlock World::getBlock(const Vector3& position)
{
    Chunk::Chunklet_Position    chunkPosition   = Maths::worldToChunkletPos(position);
    Block::Small_Position       blockPosition   = Maths::blockToSmallBlockPos(Maths::worldToBlockPos(position));

    Chunk::Section* chunk = m_chunks.get(chunkPosition);

    if (chunk)
        return chunk->qGetBlock(blockPosition);
    else
        return Block::ID::Air;
}


void World::addSectionUpdate(Chunk::Section* section) {
    m_sectionsToUpdate.emplace_back(section);
}

void World::regenerateChunks()
{
    for(New_Block& newBlock : m_newBlocks) {
        Chunk::Chunklet_Position    chunkPosition = Maths::worldToChunkletPos(newBlock.position);
        Block::Position       blockPosition = Maths::worldToBlockPos(newBlock.position);
        Chunk::Full_Chunk*          chunkFull = m_chunks.get({chunkPosition.x, chunkPosition.z});

        chunkFull->setBlock(blockPosition, newBlock.block, true);
    }

    for(auto& sect : m_sectionsToUpdate) {
        sect->makeMesh();
    }
    m_sectionsToUpdate.clear();
    m_newBlocks.clear();
}

void World::buffer(const Camera& camera)
{
    bool isMeshMade = false;
    for (int32_t x = 0 ; x < m_worldSize; x++)
    {
        for (int32_t z = 0; z < m_worldSize; z++)
        {
            Chunk::Full_Chunk* chunk = m_chunks.get({x, z});
            if (chunk)
            {
                if(chunk->tryGen(camera))
                {
                    isMeshMade = true;
                    break;
                }
            }
        }
        if (isMeshMade)
        {
            break;
        }
    }
}

void World::draw(Renderer::Master& renderer, const Camera& camera)
{
    m_facesDrawn = 0;
    for (int32_t x = 0 ; x < m_worldSize; x++)
    {
        for (int32_t z = 0; z < m_worldSize; z++)
        {
            Chunk::Full_Chunk* chunk = m_chunks.get({x, z});
            if (chunk)
            {
                m_facesDrawn += chunk->draw(renderer, camera);
            }
        }
    }
}

void World::drawWorld(Renderer::Master& renderer, const Camera& camera)
{
    if (!m_newBlocks.empty())
        regenerateChunks();

    draw(renderer, camera);
    buffer(camera);
}
