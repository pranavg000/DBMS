//
// Created by Yogesh Kumar on 03/02/20.
//

#include "HeaderFiles/TableManager.h"

TableManager::TableManager(std::string baseURL_):baseURL(std::move(baseURL_)){}

TableManagerResult TableManager::open(const std::string& tableName, std::shared_ptr<Table>& table){
    if(tableMap.find(tableName) == tableMap.end()){
        return TableManagerResult::tableNotFound;
    }

    table = tableMap[tableName];
    if(table == nullptr){
        try{
            table = std::make_shared<Table>(tableName,
                                            getFileName(tableName, TableFileType::baseTable));
            table->loadMetadata();
        }
        catch(...){
            return TableManagerResult::openingFaliure;
        }
        tableMap[tableName] = table;
    }

    return TableManagerResult::openedSuccessfully;
}

TableManagerResult TableManager::create(const std::string& tableName,
                                        std::vector<std::string>&& columnNames_,
                                        std::vector<DataType>&& columnTypes_,
                                        std::vector<uint32_t>&& columnSize_){
    if(tableMap.find(tableName) != tableMap.end()){
        return TableManagerResult::tableAlreadyExists;
    }
    std::shared_ptr<Table> table;
    try{
        table = std::make_shared<Table>(tableName, getFileName(tableName, TableFileType::baseTable));
    }catch(...){
        return TableManagerResult::tableCreationFaliure;
    }


    // Store metadata in first page
    table->createColumns(std::move(columnNames_), std::move(columnTypes_), std::move(columnSize_));
    table->storeMetadata();
    tableMap[tableName] = table;
    return TableManagerResult::tableCreatedSuccessfully;
}

TableManagerResult TableManager::drop(const std::string& tableName){
    std::shared_ptr<Table> table;
    auto res = open(tableName, table);
    if(res != TableManagerResult::openedSuccessfully){
        return res;
    }
    // if(table.drop()) return TableManagerResult::droppingFaliure;
    return TableManagerResult::droppedSuccessfully;
}

TableManagerResult TableManager::close(const std::string& tableName){
    std::shared_ptr<Table> table;
    if(tableMap.count(tableName) == 0){
        return TableManagerResult::tableNotFound;
    }
    if(tableMap[tableName] != nullptr){
        if(!tableMap[tableName]->close()){
            return TableManagerResult::closingFaliure;
        }
    }
    return TableManagerResult::closedSuccessfully;
}

TableManagerResult TableManager::closeAll(){
    for(auto& table: tableMap){
        if(table.second->tableOpen){
            table.second->close();
            table.second.reset();
        }
    }
    tableMap.clear();
    return TableManagerResult::closedSuccessfully;
}

std::string TableManager::getFileName(const std::string& tableName, TableFileType type){
    switch(type){
        case TableFileType::indexFile:
            return baseURL + "/" + tableName + "_" + ".idx";
            break;
        case TableFileType::baseTable:
            return baseURL + "/" + tableName + ".bin";
            break;
    }
}