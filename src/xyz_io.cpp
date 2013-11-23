/*
 * pts_io.cpp
 *
 *  Created on: May 21, 2012
 *      Author: asher
 */

#include <pcl/io/xyz_io.h>
#include <istream>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

pcl::XYZAsciiReader::XYZAsciiReader()
{
}



pcl::XYZAsciiReader::~XYZAsciiReader()
{
}



int pcl::XYZAsciiReader::readHeader(const std::string & file_name, pcl::PCLPointCloud2 & cloud,
                               Eigen::Vector4f & origin, Eigen::Quaternionf & orientation,
                               int & file_version, int & data_type, unsigned int & data_idx,
                               const int offset)
{

  boost::filesystem::path fpath  = file_name;

  if ( !boost::filesystem::exists(fpath) ){
    pcl::console::print_error("[XYZAscciReader] File %s does not exist.\n", file_name.c_str());
    return -1;
  }
  if ( fpath.extension().string() != ".xyz"){
    pcl::console::print_error("[XYZAscciReader] File does not have xyz extension. \n");
    return -1;
  }

  origin = Eigen::Vector4f::Zero();
  orientation = Eigen::Quaternionf();

 {
  pcl::PCLPointField f;
  f.datatype = pcl::PCLPointField::FLOAT32;
  f.count= 1;
  f.name="x";
  cloud.fields.push_back(f);
  }

  {
  pcl::PCLPointField f;
  f.datatype = pcl::PCLPointField::FLOAT32;
  f.count= 1;
  f.name="y";
  f.offset =4;
  cloud.fields.push_back(f);
  }
  {
  pcl::PCLPointField f;
  f.datatype = pcl::PCLPointField::FLOAT32;
  f.count= 1;
  f.name="z";
  f.offset =8;
  cloud.fields.push_back(f);
  }

 cloud.point_step= 12;

  std::fstream ifile(file_name.c_str(), std::fstream::in);
  std::string line;
  int total=0;
  while(std::getline(ifile,line)){
    boost::algorithm::trim(line);
    if (line.find_first_not_of("#") !=0 ) continue;
    std::vector<std::string> tokens;
    boost::algorithm::split(tokens, line,boost::algorithm::is_any_of(", \n\r\t"), boost::algorithm::token_compress_on);
    if (tokens.size() ==3) total++;
  }
  cloud.height=1;
  cloud.width = total;
  ifile.close();

  return cloud.height * cloud.width;
}


int pcl::XYZAsciiReader::read(const std::string & file_name, pcl::PCLPointCloud2 & cloud,
                         Eigen::Vector4f & origin, Eigen::Quaternionf & orientation,
                         int & file_version, const int offset)
{

  int  data_type;
  unsigned int data_idx;
  if ( this->readHeader(file_name,cloud,origin,orientation,file_version, data_type, data_idx, offset) <0 ) return -1;
  cloud.data.resize(cloud.height*cloud.width*cloud.point_step);

  std::string line;
  std::fstream ifile(file_name.c_str(), std::fstream::in);

    int total=0;

    uint32_t ptoffset=0;
    while(std::getline(ifile,line)){
      boost::algorithm::trim(line);
      if (line.find_first_not_of("#") !=0 ) continue;
         std::vector<std::string> tokens;
         boost::algorithm::split(tokens, line,boost::algorithm::is_any_of(", \n\r\t"), boost::algorithm::token_compress_on);
         if (tokens.size() !=3) continue;
       std::stringstream ss(line);
       float x=0,y=0,z=0;
       ss >> x ;
       ss >> y ;
       ss >> z ;
       if ((x==0) && (y==0) && (z==0)){
         x=y=z = std::numeric_limits<float>::quiet_NaN();
       }
       float *data = ( (float *) ( cloud.data.data() + ptoffset) )  ;
       *( ( data    ) )  = x;
       *( ( data +1 ) )  =y;
       *( ( data +2 ) )  =z;
       ptoffset+= cloud.point_step;
    }
  return cloud.width*cloud.height;
}

