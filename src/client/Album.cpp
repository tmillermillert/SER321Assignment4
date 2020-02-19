#include <string>
#include <cmath>
#include <json/json.h>
#include <map>
#include <vector>
#include "Album.hpp"
#include <utility>
#include <iostream>

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
Album::Album(string name, string artist, map<string, Track> tracks, string image,
      vector<string> genres, string runTime, string summary){
   this->name = name;
   this->artist = artist;
   this->tracks = tracks;
   this->image = image;
   this->genres = genres;
   this->runTime = runTime;
   this->summary = summary;
}
Album::Album(){
   name = "";
   artist = "";
   tracks = map<string, Track>();
   image = "";
   genres = vector<string>();
   runTime = "";
   summary = "";

}

Album::~Album(){
   name = "";
   artist = "";
   tracks.clear();
   image = "";
   genres.clear();
   runTime = "";
   summary = "";
}

Album::Album(const Json::Value& jsonObj){
   Json::Value::Members mbr = jsonObj.getMemberNames();
   for(vector<string>::const_iterator i = mbr.begin(); i!= mbr.end(); i++){
      //cout << *i << " " << endl;
      Json::Value jsonM = jsonObj[*i];
      if(*i=="name"){
         name = jsonM.asString();
      }else if(*i=="artist"){
         artist = jsonM.asString();
      }else if(*i=="tracks"){
         int size = jsonM["track"].size();
         for(int j = 0; j < size; j++){
            Track track = Track(jsonM["track"][j]);
            tracks.insert(pair<string, Track>(track.track, track));
         }
      }else if(*i=="image"){
         image = jsonM[3]["#text"].asString();
      }else if(*i=="tags"){
         int size = jsonM["tag"].size();
         for(int j = 0; j < size; j++){
            genres.emplace_back(jsonM["tag"][j]["name"].asString());
         }
      }
      else if(*i=="wiki"){
         summary = jsonM["summary"].asString();
      }
      else if(*i=="nameMTM"){
         name = jsonM.asString();
      }
      else if(*i=="artistMTM"){
         artist = jsonM.asString();
      }
      else if(*i=="tracksMTM"){
         for(int j = 0; j < jsonM.size(); j++){
            Track track  = Track(jsonM[j]);
            tracks.insert(pair<string, Track>(track.track, track));
         }
      }
      else if(*i=="imageMTM"){
         image = jsonM.asString();
      }
      else if(*i=="genresMTM"){
         for(int j = 0; j< jsonM.size(); j++){
            genres.emplace_back(jsonM[j].asString());
         }
      }
      else if(*i=="runTimeMTM"){
         runTime = jsonM.asString();
      }
      else if(*i=="summaryMTM"){
         summary = jsonM.asString();
      }

   }
   if(runTime != ""){
      return;
   }
   int runTimeInt = 0;
   for (auto const& track : tracks){
      runTimeInt += track.second.duration;
   }
   int hours = runTimeInt / 3600;
   runTimeInt %= 3600;
   int minutes = runTimeInt / 60;
   runTimeInt %= 60;
   int seconds = runTimeInt;
   char buffer[9];
   sprintf(buffer, "%02d:%02d:%02d", hours, minutes, seconds);
   runTime = buffer;
}
Album::Album(string jsonString){
   Json::Reader reader;
   Json::Value jsonObj;
   bool parseSuccess = reader.parse(jsonString, jsonObj, false);
   if(parseSuccess){
      runTime = "";
      cout << "successful parse: "<< endl;
      Json::Value::Members mbr = jsonObj.getMemberNames();
      for(vector<string>::const_iterator i = mbr.begin(); i!= mbr.end(); i++){
         //cout << *i << " " << endl;
         Json::Value jsonM = jsonObj[*i];
         if(*i=="name"){
            name = jsonM.asString();
         }else if(*i=="artist"){
            artist = jsonM.asString();
         }else if(*i=="tracks"){
            int size = jsonM["track"].size();
            for(int j = 0; j < size; j++){
               Track track = Track(jsonM["track"][j]);
               tracks.insert(pair<string, Track>(track.track, track));
            }
         }else if(*i=="image"){
            image = jsonM[3]["#text"].asString();
         }else if(*i=="tags"){
            int size = jsonM["tag"].size();
            for(int j = 0; j < size; j++){
               genres.emplace_back(jsonM["tag"][j]["name"].asString());
            }
         }
         else if(*i=="wiki"){
            summary = jsonM["summary"].asString();
         }
         else if(*i=="nameMTM"){
            name = jsonM.asString();
         }
         else if(*i=="artistMTM"){
            artist = jsonM.asString();
         }
         else if(*i=="tracksMTM"){
            for(int j = 0; j < jsonM.size(); j++){
               Track track  = Track(jsonM[j]);
               tracks.insert(pair<string, Track>(track.track, track));
            }
         }
         else if(*i=="imageMTM"){
            image = jsonM.asString();
         }
         else if(*i=="genresMTM"){
            for(int j = 0; j< jsonM.size(); j++){
               genres.emplace_back(jsonM[j].asString());
            }
         }
         else if(*i=="runTimeMTM"){
            runTime = jsonM.asString();
         }
         else if(*i=="summaryMTM"){
            summary = jsonM.asString();
         }

      }
      if(runTime != ""){
         return;
      }
      int runTimeInt = 0;
      for (auto const& track : tracks){
         runTimeInt += track.second.duration;
      }
      int hours = runTimeInt / 3600;
      runTimeInt %= 3600;
      int minutes = runTimeInt / 60;
      runTimeInt %= 60;
      int seconds = runTimeInt;
      char buffer[9];
      sprintf(buffer, "%02d:%02d:%02d", hours, minutes, seconds);
      runTime = buffer;
   }else{
      cout << "Album constructor parse error with input: " <<
         jsonString << endl;
   }
}
string Album::toJsonString(){
   Json::Value jsonLib;
   jsonLib["nameMTM"] = name;
   jsonLib["artistMTM"] = artist;
   int j = 0;
   for (auto track : tracks){
      jsonLib["tracksMTM"][j++] = track.second.toJson();
   }
   jsonLib["imageMTM"] = image;
   for (int i = 0; i < genres.size(); i++){
      jsonLib["genresMTM"][i] = genres[i];
   }
   jsonLib["runTimeMTM"] = runTime;
   jsonLib["summaryMTM"] = summary;
   return jsonLib.toStyledString();
}
Json::Value Album::toJson(){
   Json::Value jsonLib;
   jsonLib["nameMTM"] = name;
   jsonLib["artistMTM"] = artist;
   int j = 0;
   for (auto track : tracks){
      jsonLib["tracksMTM"][j++] = track.second.toJson();
   }
   jsonLib["imageMTM"] = image;
   for (int i = 0; i < genres.size(); i++){
      jsonLib["genresMTM"][i] = genres[i];
   }
   jsonLib["runTimeMTM"] = runTime;
   jsonLib["summaryMTM"] = summary;
   return jsonLib;
}
void Album::fromJson(Json::Value json){
   *this = Album(json);
}
void Album::setValues(string name, string artist, map<string, Track> tracks, 
      string image, vector<string> genres, string runTime,
      string summary){
   this->name = name;
   this->artist = artist;
   this->tracks = tracks;
   this->image = image;
   this->genres = genres;
   this->runTime = runTime;
   this->summary = summary;
}

void Album::addTrack(Track track){
   tracks.erase(track.track);
   tracks.insert(pair<string, Track>(track.track, track));
}

bool Album::removeTrack(string name){
   int elementsRemoved = tracks.erase(name);
   if(elementsRemoved == 0){
      return false;
   }
   else{
      return true;
   }
}

Track Album::get(string trackName){
   map<string, Track>::iterator it = tracks.find(trackName);
   if(it == tracks.end()){
      return Track();
   }
   else{
      return it->second;
   }
}

void Album::print(){
   cout << "Name: " << this->name << endl 
      << "Artist: " << this->artist << endl
      << "Tracks: " << endl;
   for(auto const& track: tracks){
      cout << track.second.track << endl;
   }
   cout << "image: " << this->image << endl
      << "genres: " << endl;
   for(auto const & genre : genres){
      cout << genre << endl;
   }
   cout << "Run Time: " << runTime << endl
      << "Summary: " << summary << endl;
}

