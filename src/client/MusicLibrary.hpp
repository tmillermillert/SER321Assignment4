#include "Album.hpp"
#include <string>
#include <map>
#include <vector>
#include <json/json.h>

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
 * @version February 2020
 */
class MusicLibrary {
protected:
   map<string, Album> albums;

public:
   MusicLibrary();
   ~MusicLibrary();

   bool initLibraryFromJsonFile(string jsonFileName);
   bool toJsonFile(string jsonFileName);
   Album get(string name);
   Track getTrack(string albumName, string trackName);
   vector<string> getAlbums();
   void addTrack(string albumName, Track track, Album searchedAlbum);
   void addAlbum(Album album);
   bool removeAlbum(string albumName);
   bool removeTrack(string albumName, string trackName);
};
