/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codex_line_1.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsutormi <dsutormi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 18:47:33 by dsutormi          #+#    #+#             */
/*   Updated: 2026/08/25 18:47:43 by dsutormi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

static t_node	*ft_make_node(int id, uint32_t val)
{
	t_node		*node;

	node = malloc(sizeof(t_node) * 1);
	if (node == NULL)
		return (NULL);
	node->left = NULL;
	node->right = NULL;
	node->parent = NULL;
	node->value = val;
	node->id = id;
	return (node);
}

static void	ft_delone_tree(t_node *node_to_del)
{
	if (node_to_del->parent == NULL)
	{
		node_to_del->left = NULL;
		node_to_del->right = NULL;
		free(node_to_del);
		node_to_del = NULL;
		return ;
	}
	if (node_to_del->parent->left == node_to_del)
		node_to_del->parent->left = NULL;
	else if (node_to_del->parent->right == node_to_del)
		node_to_del->parent->right = NULL;
	free(node_to_del);
}

static void	ft_iter_tree(t_node **tree, t_node *node)
{
	t_node	*left;
	t_node	*right;

	if (node == NULL)
		return ;
	left = node->left;
	right = node->right;
	node->left = NULL;
	node->right = NULL;
	node->parent = NULL;
	ft_iter_tree(tree, left);
	ft_iter_tree(tree, right);
	ft_append_tree(tree, node);
}

t_node	*ft_delrestruct_tree(t_node **tree, t_node *node_to_del)
{
	t_node	*left;
	t_node	*right;

	left = NULL;
	right = NULL;
	if (node_to_del == NULL)
		return (*tree);
	if (node_to_del->left)
	{
		node_to_del->left->parent = NULL;
		left = node_to_del->left;
	}
	if (node_to_del->right)
	{
		node_to_del->right->parent = NULL;
		right = node_to_del->right;
	}
	ft_delone_tree(node_to_del);
	if (*tree == node_to_del)
		*tree = NULL;
	ft_iter_tree(tree, left);
	ft_iter_tree(tree, right);
	return (*tree);
}

void	*ft_run_fifo(void *cdr)
{
	t_node		*node;
	int			i;

	node = NULL;
	i = 0;
	while (((t_coder *)cdr)[0].s->pr_stat)
	{
		if (((t_coder *)cdr)[i].ready == 2)
		{
			pthread_mutex_lock(&((t_coder *)cdr)[0].s->line_mtx);
			if (!strcmp(((t_coder *)cdr)[0].s->scheduler, "fifo"))
				node = ft_make_node(i, ((t_coder *)cdr)[i].ms_finished);
			else if (!strcmp(((t_coder *)cdr)[0].s->scheduler, "edf"))
				node = ft_make_node(i, ((t_coder *)cdr)[i].bur_c_ms);
			ft_append_tree(&((t_coder *)cdr)[0].s->fifo_head, node);
			pthread_mutex_lock(&(((t_coder *)cdr)[node->id].status_mtx));
			((t_coder *)cdr)[i].ready = 0;
			pthread_mutex_unlock(&(((t_coder *)cdr)[node->id].status_mtx));
			pthread_mutex_unlock(&((t_coder *)cdr)[0].s->line_mtx);
		}
		i = ((i + 1) % ((t_coder *)cdr)[0].s->number_of_coders);
		usleep(1);
	}
	return (NULL);
}

//
// void ft_print_tree(node_t* root, int space) {
// 	if (root == NULL)
// 		return;
// 	space += 2;
// 	ft_print_tree(root->right, space);
// 	// printf("\n");
// 	for (int i = 2; i < space; i++)
// 		printf(" ");
// 	// printf("%d\n", root->id);
// 	if (root->parent == NULL)
// 		printf("%d\n", root->value);
// 	else if (root->parent->left == root)
// 		printf("%d\n", root->value);
// 	else if (root->parent->right == root)
// 		printf("%d\n", root->value);
//
// 	ft_print_tree(root->left, space);
// }