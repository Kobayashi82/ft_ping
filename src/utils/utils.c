/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/16 22:51:09 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/16 23:33:46 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "utils.h"

	#include <stdio.h>
	#include <stdlib.h>

#pragma endregion

#pragma region "Lower STRCMP"

	static int ft_tolower(int c) { if (c >= 'A' && c <= 'Z') { c += 32; } return (c); }

	int ft_lstrcmp(char *s1, const char *s2) {
		if (!s1 || !s2) return (-1);
		while (*s1 && (ft_tolower(*s1) == ft_tolower(*s2))) { s1++; s2++; }
		return (*(unsigned char *)s1 - *(unsigned char *)s2);
	}

#pragma endregion

#pragma region "Lower STRTOUL"

	int ft_strtoul(const char *optarg, unsigned long *value) {
		char			*endptr;
		unsigned long	number;

		number = strtoul(optarg, &endptr, 0);

		if (*endptr) { dprintf(2, "ft_ping: invalid value (`%s' near `%s')\n", optarg, endptr); return (1); }

		*value = number;
		return (0);
	}

#pragma endregion
