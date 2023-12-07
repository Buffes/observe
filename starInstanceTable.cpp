#include "starInstanceTable.h"
#include <QFile>

StarInstanceTable::StarInstanceTable() {
    this->data_manager = DataManager::getInstance();
    m_dirty = true;
}

QByteArray StarInstanceTable::getInstanceBuffer(int *instanceCount) {
    if (m_dirty) {
        for (int i = 0; i < data_manager->m_stars.length(); i++) {
            dVector3D pos = data_manager->m_star_positions[i];
            float scale = data_manager->m_stars[i].scale;

            auto entry = calculateTableEntry(
                {(float)pos.x, (float)pos.y, (float)pos.z},
                {1, 1, 1},
                {1, 1, 1},
                QColor(255, 255, 255),
                {(float)pos.x, (float)pos.y, (float)pos.z, scale}
            );
            m_instanceData.append((char*)&entry, sizeof(entry));
        }
        m_instanceCount = data_manager->m_stars.length();
        m_dirty = false;
    }

    if (instanceCount) {
        *instanceCount = m_instanceCount;
    }

    return m_instanceData;
}
