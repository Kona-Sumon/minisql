#include"CatalogManager.h"
#include"IndexManager.h"
#include<vector>
#include<string>

void CatalogManager::initial_catalog() {
        initial_table();
        initial_index();
    }

void CatalogManager::initial_table() {
        File file = new File(tableFilename);
        if (!file.exists()) return;
        FileInputStream fis = new FileInputStream(file);
        DataInputStream dis = new DataInputStream(fis);
        std::string tmpTableName, tmpPrimaryKey;
        int tmpIndexNum, tmpAttributeNum, tmpRowNum;

        while (dis.available() > 0) {
            std::vector<Attribute>tmpAttributeVector;
            std::vector<IndexInfo>tmpIndexVector;
            tmpTableName = dis.readUTF();
            tmpPrimaryKey = dis.readUTF();
            tmpRowNum = dis.readInt();
            tmpIndexNum = dis.readInt();
            for (int i = 0; i < tmpIndexNum; i++) {
                std::string tmpIndexName, tmpAttributeName;
                tmpIndexName = dis.readUTF();
                tmpAttributeName = dis.readUTF();
                tmpIndexVector.addElement(new IndexInfo(tmpIndexName, tmpTableName, tmpAttributeName));
            }
            tmpAttributeNum = dis.readInt();
            for (int i = 0; i < tmpAttributeNum; i++) {
                std::string tmpAttributeName, tmpType;
                NumType tmpNumType;
                int tmpLength;
                bool tmpIsUnique;
                tmpAttributeName = dis.readUTF();
                tmpType = dis.readUTF();
                tmpLength = dis.readInt();
                tmpIsUnique = dis.readBoolean();
                tmpNumType = NumType.valueOf(tmpType);
                tmpAttributeVector.addElement(new Attribute(tmpAttributeName, tmpNumType, tmpLength, tmpIsUnique));
            }
            tables.put(tmpTableName, new Table(tmpTableName, tmpPrimaryKey, tmpAttributeVector, tmpIndexVector, tmpRowNum));
        }
        dis.close();
    }

void CatalogManager::initial_index() {
        File file = new File(indexFilename);
        if (!file.exists()) return;
        FileInputStream fis = new FileInputStream(file);
        DataInputStream dis = new DataInputStream(fis);
        std::string tmpIndexName, tmpTableName, tmpAttributeName;
        int tmpBlockNum, tmpRootNum;
        while (dis.available() > 0) {
            tmpIndexName = dis.readUTF();
            tmpTableName = dis.readUTF();
            tmpAttributeName = dis.readUTF();
            tmpBlockNum = dis.readInt();
            tmpRootNum = dis.readInt();
            indexes.put(tmpIndexName, new IndexInfo(tmpIndexName, tmpTableName, tmpAttributeName, tmpBlockNum, tmpRootNum));
        }
        dis.close();
    }

void CatalogManager::store_catalog() {
        store_table();
        store_index();
    }

void CatalogManager::store_table() {
        File file = new File(tableFilename);
        FileOutputStream fos = new FileOutputStream(file);
        DataOutputStream dos = new DataOutputStream(fos);
        Table tmpTable;
        Iterator<Map.Entry<std::string, Table>> iter = tables.entrySet().iterator();
        while (iter.hasNext()) {
            Map.Entry entry = iter.next();
            tmpTable = (Table) entry.getValue();
            dos.writeUTF(tmpTable.tableName);
            dos.writeUTF(tmpTable.primaryKey);
            dos.writeInt(tmpTable.rowNum);
            dos.writeInt(tmpTable.indexNum);
            for (int i = 0; i < tmpTable.indexNum; i++) {
                IndexInfo tmpIndex = tmpTable.indexVector[i];
                dos.writeUTF(tmpIndex.indexName);
                dos.writeUTF(tmpIndex.attributeName);
            }
            dos.writeInt(tmpTable.attributeNum);
            for (int i = 0; i < tmpTable.attributeNum; i++) {
                Attribute tmpAttribute = tmpTable.attributeVector[i];
                dos.writeUTF(tmpAttribute.attributeName);
                dos.writeUTF(tmpAttribute.type.get_type().name());
                dos.writeInt(tmpAttribute.type.get_length());
                dos.writeBoolean(tmpAttribute.isUnique);
            }
        }
        dos.close();
    }

void CatalogManager::store_index() {
        File file = new File(indexFilename);
        if (file.exists()) file.delete();
        FileOutputStream fos = new FileOutputStream(file);
        DataOutputStream dos = new DataOutputStream(fos);
        Index tmpIndex;
        //Enumeration<Index> en = indexes.elements();
        Iterator<Map.Entry<std::string, IndexInfo>> iter = indexes.entrySet().iterator();
        while (iter.hasNext()) {
            Map.Entry entry = iter.next();
            tmpIndex = (IndexInfo) entry.getValue();
            //tmpIndex = en.nextElement();
            dos.writeUTF(tmpIndex.indexName);
            dos.writeUTF(tmpIndex.tableName);
            dos.writeUTF(tmpIndex.attributeName);
            dos.writeInt(tmpIndex.blockNum);
            dos.writeInt(tmpIndex.rootNum);
        }
        dos.close();
    }

void CatalogManager::show_catalog() {
        show_table();
        System.out.println();
        show_index();
    }

void CatalogManager::show_index() {
        IndexInfo tmpIndex;
        Iterator<Map.Entry<std::string, IndexInfo>> iter = indexes.entrySet().iterator();
        int idx = 5, tab = 5, attr = 9;
        //System.out.println("There are " + indexes.size() + " indexes in the database: ");
        while (iter.hasNext()) {
            Map.Entry entry = iter.next();
            tmpIndex = (IndexInfo) entry.getValue();
            idx = tmpIndex.indexName.length() > idx ? tmpIndex.indexName.length() : idx;
            tab = tmpIndex.tableName.length() > tab ? tmpIndex.tableName.length() : tab;
            attr = tmpIndex.attributeName.length() > attr ? tmpIndex.attributeName.length() : attr;
        }
        std::string format = "|%-" + idx + "s|%-" + tab + "s|%-" + attr + "s|\n";
        iter = indexes.entrySet().iterator();
        System.out.printf(format, "INDEX", "TABLE", "ATTRIBUTE");
        while (iter.hasNext()) {
            Map.Entry entry = iter.next();
            tmpIndex = (IndexInfo) entry.getValue();
            System.out.printf(format, tmpIndex.indexName, tmpIndex.tableName, tmpIndex.attributeName);
        }

    }

int CatalogManager::get_max_attr_length(Table tab) {
        int len = 9;//the size of "ATTRIBUTE"
        for (int i = 0; i < tab.attributeVector.size(); i++) {
            int v = tab.attributeVector[i].attributeName.length();
            len = v > len ? v : len;
        }
        return len;
    }

void CatalogManager::show_table() {
        Table tmpTable;
        Attribute tmpAttribute;
        Iterator<Map.Entry<std::string, Table>> iter = tables.entrySet().iterator();
        //System.out.println("There are " + tables.size() + " tables in the database: ");
        while (iter.hasNext()) {
            Map.Entry entry = iter.next();
            tmpTable = (Table) entry.getValue();
            System.out.println("[TABLE] " + tmpTable.tableName);
            std::string format = "|%-" + get_max_attr_length(tmpTable) + "s";
            format += "|%-5s|%-6s|%-6s|\n";
            System.out.printf(format, "ATTRIBUTE", "TYPE", "LENGTH", "UNIQUE");
            for (int i = 0; i < tmpTable.attributeNum; i++) {
                tmpAttribute = tmpTable.attributeVector[i];
                System.out.printf(format, tmpAttribute.attributeName, tmpAttribute.type.get_type(), tmpAttribute.type.get_length(), tmpAttribute.isUnique);
            }
            if (iter.hasNext()) cout << "-------------------------------" << endl;
        }
    }

Table get_table(std::string tableName) {
        return tables.get(tableName);
    }

Index get_index(std::string indexName) {
        return indexes.get(indexName);
    }

std::string get_primary_key(std::string tableName) {
        return get_table(tableName).primaryKey;
    }

int get_row_length(std::string tableName) {
        return get_table(tableName).rowLength;
    }

int get_attribute_num(std::string tableName) {
        return get_table(tableName).attributeNum;
    }

int get_row_num(std::string tableName) {
        return get_table(tableName).rowNum;
    }

    //check
bool is_primary_key(std::string tableName, std::string attributeName) {
        if (tables.containsKey(tableName)) {
            Table tmpTable = get_table(tableName);
            return tmpTable.primaryKey.equals(attributeName);
        } else {
            cout << "The table doesn't exist" << endl;
            return false;
        }
    }

bool is_unique(std::string tableName, std::string attributeName) {
        if (tables.containsKey(tableName)) {
            Table tmpTable = get_table(tableName);
            for (int i = 0; i < tmpTable.attributeVector.size(); i++) {
                Attribute tmpAttribute = tmpTable.attributeVector[i];
                if (tmpAttribute.attributeName.equals(attributeName)) {
                    return tmpAttribute.isUnique;
                }
            }
            //if (i >= tmpTable.attributeVector.size()) {
            cout << "The attribute doesn't exist" << endl;
            return false;
            //}
        }
        cout << "The table doesn't exist" << endl;
        return false;

    }

bool is_index_key(std::string tableName, std::string attributeName) {
        if (tables.containsKey(tableName)) {
            Table tmpTable = get_table(tableName);
            if (is_attribute_exist(tableName, attributeName)) {
                for (int i = 0; i < tmpTable.indexVector.size(); i++) {
                    if (tmpTable.indexVector[i].attributeName.equals(attributeName))
                        return true;
                }
            } else {
                System.out.println("The attribute " + attributeName + " doesn't exist");
            }
        } else
            System.out.println("The table " + tableName + " doesn't exist");
        return false;
    }

bool is_index_exist(std::string indexName) {
        return indexes.containsKey(indexName);
    }

bool is_attribute_exist(std::string tableName, std::string attributeName) {
        Table tmpTable = get_table(tableName);
        for (int i = 0; i < tmpTable.attributeVector.size(); i++) {
            if (tmpTable.attributeVector[i].attributeName.equals(attributeName))
                return true;
        }
        return false;
    }

std::string get_index_name(std::string tableName, std::string attributeName) {
        if (tables.containsKey(tableName)) {
            Table tmpTable = get_table(tableName);
            if (is_attribute_exist(tableName, attributeName)) {
                for (int i = 0; i < tmpTable.indexVector.size(); i++) {
                    if (tmpTable.indexVector[i].attributeName.equals(attributeName))
                        return tmpTable.indexVector[i].indexName;
                }
            } else {
                System.out.println("The attribute " + attributeName + " doesn't exist");
            }
        } else
            System.out.println("The table " + tableName + " doesn't exist");
        return null;
    }

std::string get_attribute_name(std::string tableName, int i) {
        return tables.get(tableName).attributeVector[i].attributeName;
    }

int get_attribute_index(std::string tableName, std::string attributeName) {
        Table tmpTable = tables.get(tableName);
        Attribute tmpAttribute;
        for (int i = 0; i < tmpTable.attributeVector.size(); i++) {
            tmpAttribute = tmpTable.attributeVector[i];
            if (tmpAttribute.attributeName==attributeName)
                return i;
        }
        cout << "The attribute doesn't exist" << endl;
        return -1;
    }

FieldType get_attribute_type(std::string tableName, std::string attributeName) {
        Table tmpTable = tables.get(tableName);
        Attribute tmpAttribute;
        for (int i = 0; i < tmpTable.attributeVector.size(); i++) {
            tmpAttribute = tmpTable.attributeVector[i];
            if (tmpAttribute.attributeName==attributeName)
                return tmpAttribute.type;
        }
        cout << "The attribute doesn't exist" << endl;
        return;
    }

int get_length(std::string tableName, std::string attributeName) {
        Table tmpTable = tables.get(tableName);
        Attribute tmpAttribute;
        for (int i = 0; i < tmpTable.attributeVector.size(); i++) {
            tmpAttribute = tmpTable.attributeVector[i];
            if (tmpAttribute.attributeName==attributeName)
                return tmpAttribute.type.get_length();
        }
        cout << "The attribute doesn't exist" << endl;
        return -1;
    }

std::string get_type(std::string tableName, int i) {
        //Table tmpTable=tables.get(tableName);
        return tables.get(tableName).attributeVector[i].type.get_type().name();
    }

int get_length(std::string tableName, int i) {
        //table tmpTable=tables.get(tableName);
        return tables.get(tableName).attributeVector[i].type.get_length();
    }

void add_row_num(std::string tableName) {
        tables.get(tableName).rowNum++;
    }

void delete_row_num(std::string tableName, int num) {
        tables.get(tableName).rowNum -= num;
    }

bool update_index_table(std::string indexName, Index tmpIndex) {
        indexes.replace(indexName, tmpIndex);
        return true;
    }

bool is_attribute_exist(std::vector<Attribute> attributeVector, std::string attributeName) {
        for (int i = 0; i < attributeVector.size(); i++) {
            if (attributeVector[i].attributeName==attributeName)
                return true;
        }
        return false;
    }

    //Interface
bool create_table(Table newTable){
        tables.put(newTable.tableName, newTable);
        //indexes.put(newTable.indexes.firstElement().indexName, newTable.indexes.firstElement());
        return true;
    }

bool drop_table(std::string tableName){
        Table tmpTable = tables.get(tableName);
        for (int i = 0; i < tmpTable.indexVector.size(); i++) {
            indexes.remove(tmpTable.indexVector[i].indexName);
        }
        tables.remove(tableName);
        return true;
    }

bool create_index(Index newIndex){
        Table tmpTable = get_table(newIndex.tableName);
        tmpTable.indexVector.addElement(newIndex);
        tmpTable.indexNum = tmpTable.indexVector.size();
        indexes.put(newIndex.indexName, newIndex);
        return true;
    }

bool drop_index(std::string indexName){
        IndexInfo tmpIndex = get_index(indexName);
        Table tmpTable = get_table(tmpIndex.tableName);
        tmpTable.indexVector.remove(tmpIndex);
        tmpTable.indexNum = tmpTable.indexVector.size();
        indexes.remove(indexName);
        return true;
    }

}
