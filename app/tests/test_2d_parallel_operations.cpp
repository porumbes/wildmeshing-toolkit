#include <igl/read_triangle_mesh.h>
#include <stdlib.h>
#include <tbb/concurrent_vector.h>
#include <wmtk/ConcurrentTriMesh.h>
#include <catch2/catch.hpp>
#include <iostream>
#include "ParallelEdgeCollapse/ParallelEdgeCollapse.h"

using namespace wmtk;


TEST_CASE("parallel_shortest_edge_collapse", "[test_2d_parallel_operations]")
{
    // 0___1___2    0 __1___2
    // \  /\  /      \  |  /
    // 3\/__\/4  ==>  \ | /
    //   \  /          \|/6
    //    \/5
    std::cout << "parallel_shortest_edge_collapse" << std::endl;
    tbb::concurrent_vector<Eigen::Vector3d> v_positions(6);
    v_positions[0] = Eigen::Vector3d(-3, 3, 0);
    v_positions[1] = Eigen::Vector3d(0, 3, 0);
    v_positions[2] = Eigen::Vector3d(3, 3, 0);
    v_positions[3] = Eigen::Vector3d(0, 0, 0);
    v_positions[4] = Eigen::Vector3d(0.5, 0, 0);
    v_positions[5] = Eigen::Vector3d(0, -3, 0);
    Edge2d::ParallelEdgeCollapse m(v_positions);
    std::vector<std::array<size_t, 3>> tris = {{{0, 1, 3}}, {{1, 2, 4}}, {{3, 1, 4}}, {{3, 4, 5}}};
    m.create_mesh(6, tris);
    std::vector<ConcurrentTriMesh::Tuple> edges = m.get_edges();
    // find the shortest edge
    double shortest = std::numeric_limits<double>::max();
    ConcurrentTriMesh::Tuple shortest_edge;
    for (ConcurrentTriMesh::Tuple t : edges) {
        size_t v1 = t.vid();
        size_t v2 = m.switch_vertex(t).vid();
        if ((v_positions[v1] - v_positions[v2]).squaredNorm() < shortest) {
            shortest = (v_positions[v1] - v_positions[v2]).squaredNorm();
            shortest_edge = t;
        }
    }
    m.collapse_shortest(100);
    // the collapsed edge tuple is not valid anymore
    REQUIRE_FALSE(shortest_edge.is_valid(m));

    m.consolidate_mesh_connectivity();

    REQUIRE(m.vert_capacity() == 4);
    REQUIRE(m.tri_capacity() == 1);
}

TEST_CASE("parallel_shortest_edge_collapse_boundary_edge", "[test_2d_parallel_operations]")
{
    // 0___1___2    0 __1___2   0 __1
    // \  /\  /      \  |  /     \  |
    // 3\/__\/4  ==>  \ | / ==>   \ |
    //                 \|/5        \|6
    //

    std::cout << "parallel_shortest_edge_collapse_boundary_edge" << std::endl;
    tbb::concurrent_vector<Eigen::Vector3d> v_positions(6);
    v_positions[0] = Eigen::Vector3d(-3, 3, 0);
    v_positions[1] = Eigen::Vector3d(0, 3, 0);
    v_positions[2] = Eigen::Vector3d(3, 3, 0);
    v_positions[3] = Eigen::Vector3d(0, 0, 0);
    v_positions[4] = Eigen::Vector3d(0.5, 0, 0);
    Edge2d::ParallelEdgeCollapse m(v_positions);
    std::vector<std::array<size_t, 3>> tris = {{{0, 1, 3}}, {{1, 2, 4}}, {{3, 1, 4}}};
    m.create_mesh(5, tris);
    std::vector<wmtk::ConcurrentTriMesh::Tuple> edges = m.get_edges();
    // find the shortest edge
    double shortest = std::numeric_limits<double>::max();
    wmtk::ConcurrentTriMesh::Tuple shortest_edge;
    for (wmtk::ConcurrentTriMesh::Tuple t : edges) {
        size_t v1 = t.vid();
        size_t v2 = m.switch_vertex(t).vid();
        if ((v_positions[v1] - v_positions[v2]).squaredNorm() < shortest) {
            shortest = (v_positions[v1] - v_positions[v2]).squaredNorm();
            shortest_edge = t;
        }
    }
    m.collapse_shortest(100);
    // the collapsed edge tuple is not valid anymore
    REQUIRE_FALSE(shortest_edge.is_valid(m));
    m.write_triangle_mesh("collapsed_boundry_edge.obj");
}

TEST_CASE("parallel_shortest_edge_collapse_octocat", "[test_2d_parallel_operations]")
{
    std::cout << "parallel_shortest_edge_collapse_octocat" << std::endl;
    const std::string root(WMT_DATA_DIR);
    const std::string path = root + "/Octocat.obj";

    Eigen::MatrixXd V;
    Eigen::MatrixXi F;
    bool ok = igl::read_triangle_mesh(path, V, F);

    REQUIRE(ok);

    tbb::concurrent_vector<Eigen::Vector3d> v(V.rows());
    std::vector<std::array<size_t, 3>> tri(F.rows());
    for (int i = 0; i < V.rows(); i++) {
        v[i] = V.row(i);
    }
    for (int i = 0; i < F.rows(); i++) {
        for (int j = 0; j < 3; j++) tri[i][j] = (size_t)F(i, j);
    }
    Edge2d::ParallelEdgeCollapse m(v);
    m.create_mesh(V.rows(), tri);
    REQUIRE(m.check_mesh_connectivity_validity());
    std::cout << " is it mesh passed " << std ::endl;
    REQUIRE(m.collapse_shortest(100));
    m.write_triangle_mesh("collapsed_octocat.obj");
}

TEST_CASE("parallel_shortest_edge_collapse_circle", "[test_2d_parallel_operations]")
{
    std::cout << "parallel_shortest_edge_collapse_circle" << std::endl;
    const std::string root(WMT_DATA_DIR);
    const std::string path = root + "/circle.obj";

    Eigen::MatrixXd V;
    Eigen::MatrixXi F;
    bool ok = igl::read_triangle_mesh(path, V, F);

    REQUIRE(ok);

    tbb::concurrent_vector<Eigen::Vector3d> v(V.rows());
    std::vector<std::array<size_t, 3>> tri(F.rows());
    for (int i = 0; i < V.rows(); i++) {
        v[i] = V.row(i);
    }
    for (int i = 0; i < F.rows(); i++) {
        for (int j = 0; j < 3; j++) tri[i][j] = (size_t)F(i, j);
    }
    Edge2d::ParallelEdgeCollapse m(v);
    m.create_mesh(V.rows(), tri);
    REQUIRE(m.check_mesh_connectivity_validity());
    std::cout << " is it mesh passed " << std ::endl;
    REQUIRE(m.collapse_shortest(1000));
    m.write_triangle_mesh("collapsed_circle.obj");
}
// TEST_CASE("parallel_shortest_edge_collapse", "[test_2d_parallel_operations]")
// {
//     using namespace Edge2d;
//     // 0___1___2    0 __1___2
//     // \  /\  /      \  |  /
//     // 3\/__\/4  ==>  \ | /
//     //   \  /          \|/6
//     //    \/5


//     tbb::concurrent_vector<Eigen::Vector3d> v_positions(6);
//     v_positions[0] = Eigen::Vector3d(-3, 3, 0);
//     v_positions[1] = Eigen::Vector3d(0, 3, 0);
//     v_positions[2] = Eigen::Vector3d(3, 3, 0);
//     v_positions[3] = Eigen::Vector3d(0, 0, 0);
//     v_positions[4] = Eigen::Vector3d(0.5, 0, 0);
//     v_positions[5] = Eigen::Vector3d(0, -3, 0);
//     ParallelEdgeCollapse m(v_positions);
//     std::vector<std::array<size_t, 3>> tris = {{{0, 1, 3}}, {{1, 2, 4}}, {{3, 1, 4}}, {{3, 4,
//     5}}}; m.create_mesh(6, tris); std::vector<ConcurrentTriMesh::Tuple> edges = m.get_edges();
//     // find the shortest edge
//     double shortest = std::numeric_limits<double>::max();
//     ConcurrentTriMesh::Tuple shortest_edge;
//     for (ConcurrentTriMesh::Tuple t : edges) {
//         size_t v1 = t.vid();
//         size_t v2 = m.switch_vertex(t).vid();
//         if ((v_positions[v1] - v_positions[v2]).squaredNorm() < shortest) {
//             shortest = (v_positions[v1] - v_positions[v2]).squaredNorm();
//             shortest_edge = t;
//         }
//     }
//     m.collapse_shortest();
//     // the collapsed edge tuple is not valid anymore
//     REQUIRE_FALSE(shortest_edge.is_valid(m));
// }