/*
 * Convert.h
 *
 *  Created on: Apr 30, 2020
 *      Author: mad
 */

#ifndef INCLUDE_NEO_LOCALIZATION_CONVERT_H_
#define INCLUDE_NEO_LOCALIZATION_CONVERT_H_

#include <neo_localization/GridMap.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h> 
#include <tf2/transform_datatypes.h>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2/utils.h>
#include "rclcpp/rclcpp.hpp"
#include <tf2/LinearMath/Transform.h>
#include <nav_msgs/msg/occupancy_grid.hpp>

/*
 * Converts ROS 3D Transform to a 2.5D matrix.
 */
inline
Matrix<double, 4, 4> convert_transform_25(const tf2::Transform& trans)
{
  Matrix<double, 4, 4> res;
  res(0, 0) = trans.getBasis()[0][0];
  res(1, 0) = trans.getBasis()[1][0];
  res(0, 1) = trans.getBasis()[0][1];
  res(1, 1) = trans.getBasis()[1][1];
  res(0, 3) = trans.getOrigin()[0];
  res(1, 3) = trans.getOrigin()[1];
  res(2, 3) = tf2::getYaw(trans.getRotation());
  res(2, 2) = 1;
  res(3, 3) = 1;
  return res;
}

/*
 * Converts ROS 3D Transform to a 3D matrix.
 */
inline
Matrix<double, 4, 4> convert_transform_3(const tf2::Transform& trans)
{
  Matrix<double, 4, 4> res;
  for(int j = 0; j < 3; ++j) {
    for(int i = 0; i < 3; ++i) {
      res(i, j) = trans.getBasis()[i][j];
    }
  }
  res(0, 3) = trans.getOrigin()[0];
  res(1, 3) = trans.getOrigin()[1];
  res(2, 3) = trans.getOrigin()[2];
  res(3, 3) = 1;
  return res;
}

/*
 * Converts a grid map to a ROS occupancy map.
 */
inline
boost::shared_ptr<nav_msgs::msg::OccupancyGrid> convert_to_ros( std::shared_ptr<GridMap<float>> map,
                        Matrix<double, 3, 1> origin,
                        rclcpp::Time m_tCurrentTimeStamp)
{
  auto grid = boost::make_shared<nav_msgs::msg::OccupancyGrid>();
  tf2::Quaternion q;
  grid->header.stamp = m_tCurrentTimeStamp;
  grid->info.resolution = map->scale();
  grid->info.width = map->size_x();
  grid->info.height = map->size_y();
  grid->info.origin.position.x = origin[0];
  grid->info.origin.position.y = origin[1];
  q.setRPY(0, 0, origin[2]);
  grid->info.origin.orientation = tf2::toMsg(q);
  grid->data.resize(map->num_cells());
  for(int y = 0; y < map->size_y(); ++y) {
    for(int x = 0; x < map->size_x(); ++x) {
      grid->data[y * map->size_x() + x] = (*map)(x, y) * 100.f;
    }
  }
  return grid;
}

#endif /* INCLUDE_NEO_LOCALIZATION_CONVERT_H_ */
