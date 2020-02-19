#include <string>
#include <cmath>
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
 * Purpose: Track is a class whose properties describe a single
 * media work -- song or video/clip.
 * Ser321 Principles of Distributed Software Systems
 * see http://pooh.poly.asu.edu/Ser321
 * @author Marcus Miller mtmille5@asu.edu
 * @version February 2020
 */
class Track {
protected:
public:

   string track;
   string artist;
   string album;
   int duration;
   string rank;
   string fileName;

   Track();
   Track(string aTrack, string anArtist,
               string anAlbum, int aDuration, string aRank, string aFileName);
   Track(const Json::Value& jsonObj);
   Track(string jsonString);
   ~Track();
   string toJsonString();
   Json::Value toJson();
   void fromJson(Json::Value json);
   void setValues(string aTrack, string anArtist,
                     string anAlbum, int aDuration, string aRank, string aFileName);
   void print();
   string getTrackTime();
};
