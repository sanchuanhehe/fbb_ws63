!(function () {
	//  <root_url>/<language>/<repo>/<version>
	let currentRepoTarget = USER_DEFINED_OPTIONS.REPO_PATH
	const curRepo = HISPARK_DOCUMENTATIONS.REPOS.find(e => e.path === currentRepoTarget)
	if (!curRepo) {
		return
	}
	let currentVersion = DOCUMENTATION_OPTIONS.VERSION
	let language = DOCUMENTATION_OPTIONS.LANGUAGE
	let rootUrlPrefix = USER_DEFINED_OPTIONS.URL_ROOT_PREFIX
	let urlRoot = rootUrlPrefix ? '/' + rootUrlPrefix + '/' : '/'
	// 语言切换预留占位
	let languagePath = language ? '/' + language + '/' : '/'
	function getCurrentRepo() {
		return HISPARK_DOCUMENTATIONS.REPOS.find(e => e.path === USER_DEFINED_OPTIONS.REPO_PATH) || {}
	}
	function getcurrentVersion(versionName) {
		const repo = getCurrentRepo()
		const versionList = [...repo.branches, ...repo.tags]
		return versionList.find(e => e.name === versionName)?.path || ''
	}

	function toVersionHref(event) {
		let version = getcurrentVersion(event.target.value)
		let hrefUrl = urlRoot + currentRepoTarget + languagePath + version
		console.log(hrefUrl)
		window.location.href = hrefUrl
	}
	var versionSelect = document.getElementById('version-select')
	const versionSelector = document.querySelector('.version-selector')
	const versionOptions = document.getElementById('version-options')
	versionSelector.addEventListener('click', () => {
		if (versionOptions.style.display == 'none' || !versionOptions.style.display) {
			versionOptions.style.display = 'block'
			versionSelector.classList.add('active')
			versionSelect.focus()
		} else {
			versionOptions.style.display = 'none'
			versionSelector.classList.remove('active')
		}
	})
	versionSelect.addEventListener('blur', () => {
		setTimeout(() => {
			versionOptions.style.display = 'none'
			versionSelector.classList.remove('active')
		}, 100)
	})
	versionSelect.hidden = false
	versionSelect.onchange = toVersionHref
	versionOptions.addEventListener('click', e => {
		if (e.target.className === 'option') {
			versionSelect.value = e.target.innerText
			versionSelector.setAttribute('title', versionSelect.value)
			$('.version-readOnly').text(versionSelect.value)
			var event = new Event('change')
			versionSelect.dispatchEvent(event)
			versionOptions.style.display = 'none'
		}
	})
	let release_versions = curRepo.tags
	const releaseTitle = document.createElement('li')
	releaseTitle.setAttribute('class', 'opTitle')
	releaseTitle.innerText = 'release'
	versionOptions.append(releaseTitle)
	for (let i = 0; i < release_versions.length; i++) {
		let version = release_versions[i]
		const option = document.createElement('li')
		option.setAttribute('class', 'option')
		option.setAttribute('title', version.name)
		option.innerText = version.name
		versionOptions.append(option)
	}
	let pre_release_versions = curRepo.branches
	const previewTitle = document.createElement('li')
	previewTitle.setAttribute('class', 'opTitle')
	previewTitle.innerText = 'preview'
	versionOptions.append(previewTitle)

	for (let i = 0; i < pre_release_versions.length; i++) {
		let version = pre_release_versions[i]
		const option = document.createElement('li')
		option.setAttribute('class', 'option')
		option.setAttribute('title', version.name)
		option.innerText = version.name
		versionOptions.append(option)
	}
	versionSelect.value = [...pre_release_versions, ...release_versions].find(e => e.path === currentVersion)?.name || ''
	versionSelector.setAttribute('title', versionSelect.value)
	$('.version-readOnly').text(versionSelect.value)
	function getCurrentRepoTarget(repoName) {
		return HISPARK_DOCUMENTATIONS.REPOS.find(e => e.name === repoName) || {}
	}
	function toRepoHref(event) {
		let repo = getCurrentRepoTarget(event.target.value)
		let hrefUrl = urlRoot + repo.path + languagePath + repo.default_version
		console.log(hrefUrl)
		window.location.href = hrefUrl
	}
	var repoSelect = document.getElementById('repo-select')
	const repoSelector = document.querySelector('.repo-selector')
	const repoOptions = document.getElementById('repo-options')
	repoSelector.addEventListener('click', () => {
		if (repoOptions.style.display == 'none' || !repoOptions.style.display) {
			repoOptions.style.display = 'block'
			repoSelector.classList.add('active')
			repoSelect.focus()
		} else {
			repoOptions.style.display = 'none'
			repoSelector.classList.remove('active')
		}
	})
	repoSelect.addEventListener('blur', () => {
		setTimeout(() => {
			repoOptions.style.display = 'none'
			repoSelector.classList.remove('active')
		}, 100)
	})
	repoOptions.addEventListener('click', e => {
		if (e.target.className === 'option') {
			repoSelect.value = e.target.innerText
			repoSelector.setAttribute('title', repoSelect.value)
			$('.repo-readOnly').text(repoSelect.value)
			var event = new Event('change')
			repoSelect.dispatchEvent(event)
			repoOptions.style.display = 'none'
		}
	})
	repoSelect.hidden = false
	repoSelect.onchange = toRepoHref
	let repoTargets = HISPARK_DOCUMENTATIONS.REPOS.map(e => {
		return { text: e.name, value: e.path }
	})
	// DOCUMENTATION_VERSIONS.REPO_TARGETS
	repoSelect.value = repoTargets.find(e => e.value == currentRepoTarget)?.text || ''
	repoSelector.setAttribute('title', repoSelect.value)
	$('.repo-readOnly').text(repoSelect.value)
	if (repoTargets.length === 0) {
		$('.repo-select').hide()
		$('.version-select').addClass('only-version')
	}

	for (let i = 0; i < repoTargets.length; i++) {
		let repoTarget = repoTargets[i]
		const option = document.createElement('li')
		option.setAttribute('class', 'option')
		option.setAttribute('title', repoTarget.text)
		option.innerText = repoTarget.text
		repoOptions.append(option)
	}
	window.onpopstate = function () {
		if (location.href.match(/\#$/)) {
			$('.wy-nav-content').scrollTop(0)
		}
	}
})()
