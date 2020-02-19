#include "MusicLibrary.hpp"
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>

using namespace std;
/**
 * Copyright 2020 Marcus Miller,
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * Purpose: MusicLibrary is a class defining the interface between clients
 * and the server. The server implementation of MusicLibrary
 * provides storage for description of multiple albums.
 * Ser321 Principles of Distributed Software Systems
 * see http://pooh.poly.asu.edu/Ser321
 * @author Marcus Miller mtmille5@asu.edu
 * @version Feburary 2020
 */

MusicLibrary::MusicLibrary(){
   initLibraryFromJsonFile("album.json");
}

MusicLibrary::~MusicLibrary() {
   //cout << "MediaLibrary destructor.\n";
   albums.clear();
}

bool MusicLibrary::initLibraryFromJsonFile(string jsonFileName){
   bool ret = false;
   Json::Reader reader;
   Json::Value root;
   std::ifstream json(jsonFileName.c_str(), std::ifstream::binary);
   bool parseSuccess = reader.parse(json,root,false);
   if(parseSuccess){
      for(int i = 0; i < root.size(); i++){
         //cout << *i << " " << endl;
         Json::Value jsonAlbum = root[i];
         Album album(jsonAlbum);
         albums.insert(pair<string, Album>(album.name, album));
         cout << "adding ";
         album.print();
      }
      ret = true;
   }
   return ret;
}

bool MusicLibrary::toJsonFile(string jsonFileName){
   bool ret = false;
   Json::Value jsonLib;
   map<string, Album>::iterator it = albums.begin();
   for(int i = 0; i < albums.size(); i++){
      //string key = i->first;
      //cout << key << " " << endl;
      Json::Value jsonMedia = it->second.toJson();
      //jsonLib[key] = jsonMedia;
      jsonLib[i] = jsonMedia;
      it++;
   }
   Json::StyledStreamWriter ssw("  ");
   std::ofstream jsonOutFile(jsonFileName.c_str(), std::ofstream::binary);
   ssw.write(jsonOutFile, jsonLib);
   return true;
}

Album MusicLibrary::get(string albumTitle){
   //cout << "get: " << albumTitle << endl;
   map<string, Album>::iterator it = albums.find(albumTitle);
   if(it != albums.end()){
      return it->second;
   }
   return Album();
}

Track MusicLibrary::getTrack(string albumName, string trackName){
   map<string, Album>::iterator it = albums.find(albumName);
   Album album;
   if(it != albums.end()){
      album = it->second;
   }
   else{
      return Track();
   }
   return album.get(trackName);
}

vector<string> MusicLibrary::getAlbums(){
   //cout << "getAlbums: " << endl;
   vector<string> myVec;
   for(map<string,Album>::iterator it = albums.begin();
         it != albums.end();++it){
      myVec.push_back(it->first);
      //cout << it->first << "\n";
   }
   //cout << "getAlbums end" << endl;
   return myVec;
}

bool MusicLibrary::removeAlbum(string albumTitle){
   int elementsRemoved( albums.erase(albumTitle));
   if(elementsRemoved == 1){
      return true;
   }
   else{
      return false;
   }

}

bool MusicLibrary::removeTrack(string albumName, string trackName){
   return albums[albumName].tracks.erase(trackName);
   /*
   Album album = get(albumName);
   if(album.name == ""){
      return false;
   }
   if(album.removeTrack(trackName)){
      return true;
   }
   else{
      return false;
   }
   */
}

void MusicLibrary::addTrack(string albumName, Track track, Album searchedAlbum){
   Album album = get(albumName);
   if (album.name == ""){
      album = Album();
      album.name = searchedAlbum.name;
      album.artist = searchedAlbum.artist;
      album.image = searchedAlbum.image;
      album.genres = searchedAlbum.genres;
      album.summary = searchedAlbum.summary;
      album.runTime = searchedAlbum.runTime;
      album.tracks.insert(pair<string, Track>(track.track, track));
      addAlbum(album);
   }
   album.addTrack(track);
   addAlbum(album);
}

void MusicLibrary::addAlbum(Album album){
   albums.erase(album.name);
   albums.insert(pair<string, Album>(album.name, album));
}
