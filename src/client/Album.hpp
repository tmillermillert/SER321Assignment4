#include <string>
#include <cmath>
#include <json/json.h>
#include <map>
#include <vector>
#include "Track.hpp"

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
 * Purpose: Album is a class whose properties describe multiple
 * media works -- songs or videos/clips.
 * Ser321 Principles of Distributed Software Systems
 * see http://pooh.poly.asu.edu/Ser321
 * @author Marcus Miller mtmille5@asu.edu
 * @version February 2020
 */
class Album {
public:
   //the title of the album
   string name;
   //the artist or group name
   string artist;
   //a collection of Track objects representing the music tracks in the album
   map<string, Track> tracks;
   //a string URL to the Last.fm provide image
   string image;
   //an option collection of genre names
   vector<string> genres;
   //the sum of the tracks run times
   string runTime;
   //an optional summary description of the album
   string summary;

   Album(string name, string artist, map<string, Track> tracks, string image,
		   vector<string> genres, string runTime, string summary);
   ~Album();
   Album();
   Album(const Json::Value& jsonObj);
   Album(string jsonString);
   string toJsonString();
   Json::Value toJson();
   void fromJson(Json::Value json);
   void setValues(string name, string artist, map<string, Track> tracks, 
		   string image, vector<string> genres, string Runtime,
		   string summary);
   void addTrack(Track track);
   bool removeTrack(string name);
   void print();
   Track get(string trackName);
};
