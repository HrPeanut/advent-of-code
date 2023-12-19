#ifndef TRAK_GRAPH_H
#define TRAK_GRAPH_H

#include <vector>
#include <cassert>

namespace trak {

    template<typename T>
    concept is_weighted = std::integral<T>
        || std::floating_point<T>
        || requires(T value) {{ value.weight() } -> std::integral; }
        || requires(T value) {{ value.weight() } -> std::floating_point; };

    template<typename T, typename E = int>
    class basic_graph;

    template<typename E>
    class basic_graph_edge {
    public:
        using size_type = std::size_t;

        using value_type = E;
        using reference = E&;
        using const_reference = const E&;

        template<typename... Args>
        explicit basic_graph_edge(size_type to, Args&& ... args) : _to(to), _value(std::forward<Args>(args)...) {}

        basic_graph_edge& operator=(const E& val) {
            _value = val;
            return *this;
        }

        basic_graph_edge& operator=(E&& val) {
            _value = std::move(val);
            return *this;
        }

        [[nodiscard]] reference value() { return _value; }
        [[nodiscard]] const_reference value() const { return _value; }

        [[nodiscard]] size_type to() const { return _to; }

        [[nodiscard]] explicit operator reference () { return _value; }
        [[nodiscard]] explicit operator const_reference () const { return _value; }

    private:
        size_type _to;
        value_type _value;
    };

    template<typename T, typename E>
    class basic_graph_node {
        friend class basic_graph<T>;

    public:
        using size_type = std::size_t;

        using value_type = T;
        using reference = T&;
        using const_reference = const T&;

        using edge_type = basic_graph_edge<E>;
        using edge_list = std::vector<edge_type>;

        basic_graph_node() : _value() {}
        explicit basic_graph_node(const_reference val) : _value(val) {}

        basic_graph_node& operator=(const T& val) {
            _value = val;
            return *this;
        }

        basic_graph_node& operator=(T&& val) {
            _value = std::move(val);
            return *this;
        }

        [[nodiscard]] reference value() { return _value; }
        [[nodiscard]] const_reference value() const { return _value; }

        [[nodiscard]] const edge_list& edges() const { return _edges; }

        [[nodiscard]] explicit operator reference () { return _value; }
        [[nodiscard]] explicit operator const_reference () const { return _value; }

    protected:
        template<typename... Args>
        void add_edge(size_type to, Args&&... args) {
            _edges.emplace_back(to, std::forward<Args>(args)...);
        }

        edge_list _edges;
        value_type _value;
    };

    template<typename T, typename E>
    class basic_graph {
    public:
        using size_type = std::size_t;
        using node_type = basic_graph_node<T, E>;
        using node_list = std::vector<node_type>;

        explicit basic_graph(size_type node_count) : _nodes(node_count) {}
        basic_graph(size_type node_count, const T& val) : _nodes(node_count, node_type{val}) {}

        const node_type& operator[](size_type i) const { return _nodes[i]; }
        node_type& operator[](size_type i) { return _nodes[i]; }

        [[nodiscard]] const node_list& nodes() const { return _nodes; }
        [[nodiscard]] node_list& nodes() { return _nodes; }

        template<typename... Args>
        void add_edge(size_type from, size_type to, Args&& ... args) {
            assert(from < _nodes.size());
            assert(to < _nodes.size());
            _nodes[from].add_edge(to, std::forward<Args>(args)...);
        }

    private:
        node_list _nodes;
    };

    template<typename T, typename E = int>
    using graph = basic_graph<T, E>;
}

#endif //TRAK_GRAPH_H
